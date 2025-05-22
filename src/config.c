#include "config.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TOKEN_NULL,
  TOKEN_SECTION,
  TOKEN_KVP,
  TOKEN_KEY,
  TOKEN_VALUE,
} TokenType;

typedef struct {
  TokenType type;
  size_t start;
  size_t end;
} Token;

static Token NewToken(TokenType type) {
  return (Token){.type = type, .start = 0, .end = 0};
}

static char *input;
static size_t len;
static size_t pos = 0;
static size_t saved_len;
static size_t saved_pos = 0;

static char Peek() { return (pos >= len) ? EOF : input[pos]; }
static bool Advance() { return (pos + 1 > len) ? false : pos++; }
static bool Expect(char c) { Peek() == c ? Advance() : false; }
static void ConsumeUntil(char end) { while (Peek() != end && Advance()); }

static Token LexSection() {
  Token token = NewToken(TOKEN_SECTION);
  Expect('[');
  token.start = pos;
  ConsumeUntil(']');
  token.end = pos;
  Expect(']');
  Expect('\n');
  return token;
}

static Token LexKeyValuePair() {
  Token token = NewToken(TOKEN_KVP);
  token.start = pos;
  ConsumeUntil('\n');
  token.end = pos;
  Expect('\n');
  return token;
}

static Token NextToken() {
  switch (Peek()) {
    case EOF:
      return NewToken(TOKEN_NULL);
    case '[':
      return LexSection();
    default:
      return LexKeyValuePair();
  }
}

static Token LexValue() {
  Token token = NewToken(TOKEN_VALUE);
  Expect('=');
  token.start = pos;
  ConsumeUntil('\n');
  token.end = pos;
  Expect('\n');
  return token;
}

static Token LexKey() {
  Token token = NewToken(TOKEN_VALUE);
  token.start = pos;
  ConsumeUntil('=');
  token.end = pos;
  return token;
}

static void StartInline(Token parent) {
  saved_pos = pos;
  saved_len = len;
  pos       = parent.start;
  len       = pos + parent.end - parent.start;
}

static void EndInline() {
  pos = saved_pos;
  len = saved_len;
}

static Token NextInlineToken() {
  switch (Peek()) {
    case '=':
      return LexValue();
    default:
      return LexKey();
  }
}

static void LoadConfigFile(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Couldn't find config file");
    exit(1);
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Failed to seek to end");
    fclose(file);
    exit(1);
  }

  long length = ftell(file);
  if (length < 0) {
    perror("Failed to tell file length");
    fclose(file);
    exit(1);
  }

  if (fseek(file, 0, SEEK_SET) != 0) {
    perror("Failed to seek to start");
    fclose(file);
    exit(1);
  }

  char *buffer = malloc(length + 1);
  if (!buffer) {
    perror("Failed to allocate memory");
    fclose(file);
    exit(1);
  }

  size_t read_bytes = fread(buffer, 1, length, file);
  if (read_bytes != (size_t)length) {
    perror("Failed to read full file");
    free(buffer);
    fclose(file);
    exit(1);
  }

  buffer[length] = '\0';

  fclose(file);

  input = buffer;
  len   = length;
  pos   = 0;
}

void LoadConfig() { LoadConfigFile("gendoc.config"); }

void UnloadConfig() { free(input); }

#define GLOBAL "!global"

char *ReadConfig(const char *path) {
  pos = 0;
  char _path[200];
  strcpy(_path, path);

  char *section = strtok(_path, ".");
  char *key     = strtok(NULL, ".");

  if (key == NULL) {
    key     = section;
    section = GLOBAL;
  }

  Token token;
  char current_section[100] = GLOBAL;
  char *value               = path;
  while ((token = NextToken()).type != TOKEN_NULL) {
    if (token.type == TOKEN_SECTION) {
      if (strcmp(section, GLOBAL) == 0) {
        break;
      }
      strncpy(current_section, &input[token.start], token.end - token.start);
      current_section[token.end - token.start] = '\0';
      continue;
    }

    if (strcmp(current_section, section) == 0) {
      assert(token.type == TOKEN_KVP);
      StartInline(token);
      Token key_token   = NextInlineToken();
      size_t key_length = key_token.end - key_token.start;
      if (strncmp(&input[key_token.start], key, key_length) == 0) {
        Token value_token = NextInlineToken();
        size_t length     = value_token.end - value_token.start;
        value             = (char *)malloc(length);
        strncpy(value, &input[value_token.start], length);
        value[length] = '\0';
        EndInline();
        return value;
      }
      EndInline();
    }
  }

  return value;
}