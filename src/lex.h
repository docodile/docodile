#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
  TOKEN_TEXT,
  TOKEN_LINK,
  TOKEN_ITALICS,
  TOKEN_BOLD,
  TOKEN_BOLD_AND_ITALICS,
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
static Token TokenNew(int line) { return (Token){.type = TOKEN_UNKNOWN, .length = 0, .lexeme = NULL, .line = line}; }

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
  stream->count       = 0;
  stream->pos         = 0;

  return (Lexer){.input = input, .stream = stream, .pos = 0, .current_line = 1, .current_token = TokenNew(0)};
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

// static char *ConsumeLine(Lexer *lexer) {
//   char *buffer = malloc(MAX_LINE);

//   int count = 0;
//   char c;
//   while ((c = fgetc(lexer->input)) != EOF && c != '\n') {
//     buffer[count++] = c;
//   }
//   buffer[count] = '\0';
//   return buffer;
// }

static char *ConsumeUntilAny(Lexer *lexer, char *ends, bool inclusive) {
  char *buffer = malloc(MAX_LINE);

  int count = 0;
  char c;
  while ((c = Peek(lexer)) != EOF && strchr(ends, c) == NULL) {
    Advance(lexer);
    buffer[count++] = c;
  }
  if (inclusive) Advance(lexer);
  buffer[count] = '\0';
  return buffer;
}

static char *ConsumeUntil(Lexer *lexer, char end, bool inclusive) { return ConsumeUntilAny(lexer, &end, inclusive); }

static char *ConsumeLine(Lexer *lexer) { return ConsumeUntil(lexer, '\n', true); }

static void CommitToken(Lexer *lexer) {
  Token token = lexer->current_token;
  LOG_DEBUG("Created token on line %d: %d - %s - %d", token.line, token.type, token.lexeme, token.length);
  lexer->stream->stream[lexer->stream->count++] = token;
  lexer->current_token                          = TokenNew(lexer->current_line++);
}

static void LexHeading(Lexer *lexer) {
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

static void LexText(Lexer *lexer) {
  lexer->current_token.type   = TOKEN_TEXT;
  lexer->current_token.lexeme = ConsumeUntilAny(lexer, "[*_", false);
  lexer->current_token.length = strlen(lexer->current_token.lexeme);
  NewLine(lexer);
  CommitToken(lexer);
}

static void LexLink(Lexer *lexer) {
  lexer->current_token.type   = TOKEN_LINK;
  lexer->current_token.lexeme = ConsumeUntil(lexer, ')', true);
  lexer->current_token.length = strlen(lexer->current_token.lexeme);
  NewLine(lexer);
  CommitToken(lexer);
}

static int min(int a, int b) {
  if (a > b) return b;

  return a;
}

static void LexEmphasis(Lexer *lexer) {
  char format  = Peek(lexer);
  int opening  = Repeats(lexer, format);
  char *lexeme = ConsumeUntil(lexer, format, false);
  int closing  = Repeats(lexer, format);

  int matching = min(opening, closing);

  TokenType type;
  if (matching == 0) type = TOKEN_TEXT;
  if (matching == 1) type = TOKEN_ITALICS;
  if (matching == 2) type = TOKEN_BOLD;
  // HACK Not the proper way of doing this, but for now bold and italics is the only nested case we need to handle.
  if (matching > 2) type = TOKEN_BOLD_AND_ITALICS;

  lexer->current_token.type   = type;
  lexer->current_token.lexeme = lexeme;
  lexer->current_token.length = strlen(lexeme);

  CommitToken(lexer);
}

TokenStream *LexInline(Token token) {
  LOG_DEBUG("TEST");
  FILE *f     = fmemopen(token.lexeme, token.length, "r");
  Lexer lexer = LexerNew(f);

  char c;
  while ((c = Peek(&lexer)) != EOF && c != '\n') {
    switch (c) {
      case '[':
        LexLink(&lexer);
        break;
      case '*':
      case '_':
        LexEmphasis(&lexer);
        break;
      default:
        LexText(&lexer);
        break;
    }
  }

  return lexer.stream;
}

#endif  // LEX_H