#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

#define MAX_LINE   1000
#define MAX_TOKENS 10000

typedef enum {
  TOKEN_UNKNOWN,
  TOKEN_H1,
  TOKEN_H2,
  TOKEN_H3,
  TOKEN_H4,
  TOKEN_H5,
  TOKEN_H6,
  TOKEN_P,
} TokenType;

typedef struct {
  TokenType type;
  const char *lexeme;
  size_t length;
  size_t line;
} Token;

typedef struct {
  size_t count;
  size_t pos;
  Token *stream;
} TokenStream;

typedef struct {
  FILE *input;
  TokenStream *stream;
  size_t pos;
  Token current_token;
  size_t current_line;
} Lexer;

Token TokenStreamNext(TokenStream *stream);

Token TokenStreamNext(TokenStream *stream) { return stream->stream[stream->pos++]; }

TokenStream *Lex(FILE *input);

static void Advance(Lexer *lexer) { fgetc(lexer->input); }

static char Peek(Lexer *lexer) {
  char c = fgetc(lexer->input);

  if (c != EOF) ungetc(c, lexer->input);

  return c;
}
static TokenStream *TokenStreamNew();
static Lexer LexerNew(FILE *input) {
  TokenStream *stream = (TokenStream *)malloc(sizeof(TokenStream));
  stream->stream      = malloc(MAX_TOKENS * sizeof(Token));
  return (Lexer){.input = input, .stream = stream, .pos = 0, .current_line = 0};
}
static void NewLine(Lexer *lexer) {
  if (Peek(lexer) == '\n') {
    Advance(lexer);
  }
}
static void Whitespace(Lexer *lexer) {
  char c;
  while ((c = Peek(lexer)) == ' ') {
    Advance(lexer);
  }
}
static int Repeats(Lexer *lexer, char expected) {
  int count = 0;

  while (Peek(lexer) == expected) {
    Advance(lexer);
    count++;
  }

  return count;
}
static char *ConsumeLine(Lexer *lexer) {
  char *buffer = malloc(MAX_LINE);

  int count = 0;
  char c;
  while ((c = fgetc(lexer->input)) != EOF && c != '\n') {
    buffer[count++] = c;
  }
  buffer[count] = '\0';
  return buffer;
}

static Token TokenNew(int line) { return (Token){.type = TOKEN_UNKNOWN, .length = 0, .lexeme = NULL, .line = line}; }

static void CommitToken(Lexer *lexer) {
  Token token = lexer->current_token;
  LOG_DEBUG("Created token on line %d: %d - %s - %d", token.line, token.type, token.lexeme, token.length);
  lexer->stream->stream[lexer->stream->count++] = token;
}

static void LexHeading(Lexer *lexer) {
  lexer->current_token = TokenNew(lexer->current_line++);

  int count = Repeats(lexer, '#');
  switch (count) {
    case 1:
      lexer->current_token.type = TOKEN_H1;
      break;
    case 2:
      lexer->current_token.type = TOKEN_H2;
      break;
    case 3:
      lexer->current_token.type = TOKEN_H3;
      break;
    case 4:
      lexer->current_token.type = TOKEN_H4;
      break;
    case 5:
      lexer->current_token.type = TOKEN_H5;
      break;
    default:
      lexer->current_token.type = TOKEN_H6;
      break;
  }

  Whitespace(lexer);

  lexer->current_token.lexeme = ConsumeLine(lexer);
  lexer->current_token.length = strlen(lexer->current_token.lexeme);

  NewLine(lexer);
  CommitToken(lexer);
}

static void LexParagraph(Lexer *lexer) {
  lexer->current_token        = TokenNew(lexer->current_line++);
  lexer->current_token.type   = TOKEN_P;
  lexer->current_token.lexeme = ConsumeLine(lexer);
  lexer->current_token.length = strlen(lexer->current_token.lexeme);
  NewLine(lexer);
  CommitToken(lexer);
}

TokenStream *Lex(FILE *input) {
  Lexer lexer = LexerNew(input);

  char c;

  while ((c = Peek(&lexer)) != EOF) {
    Whitespace(&lexer);
    switch (c) {
      case '#':
        LexHeading(&lexer);
        break;
      default:
        LexParagraph(&lexer);
        break;
    }
  }

  return lexer.stream;
}

#endif  // LEX_H