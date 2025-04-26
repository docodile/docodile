#ifndef LEX_H
#define LEX_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

#define MAX_LINE   1000
#define MAX_TOKENS 10000

typedef enum {
  TOKEN_UNKNOWN,
  TOKEN_NULL,
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
} TokenType;

typedef struct {
  char *input;
  TokenType type;
  size_t start;
  size_t end;
  size_t length;
  size_t line;
} Token;

typedef struct Lexer {
  char *input;
  size_t pos;
  size_t end;
  size_t current_line;
} Lexer;

static Token TokenNew(char *input, size_t pos) {
  return (Token){.input  = input,
                 .type   = TOKEN_UNKNOWN,
                 .length = 0,
                 .start  = pos,
                 .end    = pos,
                 .line   = 0};
}

static Token TokenNull() { return (Token){.input = NULL, .type = TOKEN_NULL}; }

static void Advance(Lexer *lexer) { lexer->pos++; }

static char Peek(Lexer *lexer) {
  if (lexer->pos >= lexer->end) return '\0';
  return lexer->input[lexer->pos];
}

static Lexer LexerNew(char *input, size_t start, size_t end) {
  return (Lexer){.input = input, .pos = start, .end = end, .current_line = 1};
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

static int RepeatsMax(Lexer *lexer, char expected, int max) {
  int count = 0;

  while (Peek(lexer) == expected && count < max) {
    Advance(lexer);
    count++;
  }

  return count;
}

static int Repeats(Lexer *lexer, char expected) {
  int count = 0;

  while (Peek(lexer) == expected) {
    Advance(lexer);
    count++;
  }

  return count;
}

static size_t ConsumeUntilAny(Lexer *lexer, const char *ends, bool inclusive) {
  char c = 0;
  while ((c = Peek(lexer)) != '\0' && strchr(ends, c) == NULL) {
    Advance(lexer);
  }
  if (inclusive) Advance(lexer);
  return lexer->pos;
}

static size_t ConsumeUntil(Lexer *lexer, char end, bool inclusive) {
  return ConsumeUntilAny(lexer, &end, inclusive);
}

static size_t ConsumeLine(Lexer *lexer) {
  return ConsumeUntil(lexer, '\n', false);
}

static Token LexHeading(Lexer *lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);

  int count = Repeats(lexer, '#');
  switch (count) {
    case 1:
      token.type = TOKEN_H1;
      break;
    case 2:
      token.type = TOKEN_H2;
      break;
    case 3:
      token.type = TOKEN_H3;
      break;
    case 4:
      token.type = TOKEN_H4;
      break;
    case 5:
      token.type = TOKEN_H5;
      break;
    default:
      token.type = TOKEN_H6;
      break;
  }

  Whitespace(lexer);
  size_t start = lexer->pos;
  size_t end   = ConsumeLine(lexer);
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  NewLine(lexer);
  return token;
}

static Token LexParagraph(Lexer *lexer) {
  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_P;
  size_t start = lexer->pos;
  size_t end   = ConsumeLine(lexer);
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  NewLine(lexer);
  return token;
}

static Token LexText(Lexer *lexer) {
  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_TEXT;
  size_t start = lexer->pos;
  size_t end   = ConsumeUntilAny(lexer, "[*_\n", false);
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  NewLine(lexer);
  return token;
}

static Token LexLink(Lexer *lexer) {
  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_LINK;
  size_t start = lexer->pos;
  size_t end   = ConsumeUntil(lexer, ')', true);
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  NewLine(lexer);
  return token;
}

static int min(int a, int b) {
  if (a > b) return b;

  return a;
}

void TokenPrint(Token *token) {
  for (size_t i = token->start; i < token->end; i++) {
    putchar(token->input[i]);
  }
}

static Token LexEmphasis(Lexer *lexer) {
  char format  = Peek(lexer);
  int opening  = Repeats(lexer, format);
  size_t start = lexer->pos;
  size_t end   = ConsumeUntil(lexer, format, false);
  int closing  = Repeats(lexer, format);
  int matching = min(opening, closing);

  TokenType type;
  if (matching == 0) type = TOKEN_TEXT;
  if (matching == 1) type = TOKEN_ITALICS;
  if (matching >= 2) type = TOKEN_BOLD;

  Token token  = TokenNew(lexer->input, start);
  token.type   = type;
  token.end    = end;
  token.length = end - start;

  return token;
}

Token NextToken(Lexer *lexer) {
  char c = Peek(lexer);
  if (c == '\0') return TokenNull();

  Whitespace(lexer);
  Token token;
  switch (c) {
    case '#':
      token = LexHeading(lexer);
      break;
    default:
      token = LexParagraph(lexer);
      break;
  }
  Whitespace(lexer);
  NewLine(lexer);

  return token;
}

Token NextInlineToken(Lexer *lexer) {
  char c = Peek(lexer);
  if (c == '\0') return TokenNull();

  switch (c) {
    case '[':
      return LexLink(lexer);
    case '*':
    case '_':
      return LexEmphasis(lexer);
    default:
      return LexText(lexer);
  }
}

#endif  // LEX_H