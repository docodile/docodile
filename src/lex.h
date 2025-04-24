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

typedef struct {
  size_t count;
  size_t pos;
  Token *stream;
} TokenStream;

typedef struct Lexer {
  char *input;
  TokenStream *stream;
  size_t pos;
  size_t end;
  Token current_token;
  size_t current_line;
} Lexer;

Token TokenStreamNext(TokenStream *stream);
void TokenStreamFree(TokenStream *stream);
TokenStream *Lex(char *input);

Token TokenStreamNext(TokenStream *stream) { return stream->stream[stream->pos++]; }

void TokenStreamFree(TokenStream *stream) {
  if (stream->stream) {
    free(stream->stream);
  }

  if (stream) {
    free(stream);
  }
}

static Token TokenNew(char *input, size_t pos) {
  return (Token){.input = input, .type = TOKEN_UNKNOWN, .length = 0, .start = pos, .end = pos, .line = 0};
}

static void Advance(Lexer *lexer) { lexer->pos++; }

static char Peek(Lexer *lexer) {
  if (lexer->pos >= lexer->end) return '\0';
  return lexer->input[lexer->pos];
}

static Lexer LexerNew(char *input, size_t start, size_t end) {
  TokenStream *stream = (TokenStream *)malloc(sizeof(TokenStream));
  stream->stream      = malloc(MAX_TOKENS * sizeof(Token));
  stream->count       = 0;
  stream->pos         = 0;

  return (Lexer){.input         = input,
                 .stream        = stream,
                 .pos           = start,
                 .end           = end,
                 .current_line  = 1,
                 .current_token = TokenNew(input, start)};
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

static size_t ConsumeUntil(Lexer *lexer, char end, bool inclusive) { return ConsumeUntilAny(lexer, &end, inclusive); }

static size_t ConsumeLine(Lexer *lexer) { return ConsumeUntil(lexer, '\n', false); }

static void CommitToken(Lexer *lexer) {
  lexer->stream->stream[lexer->stream->count++] = lexer->current_token;
  lexer->current_token                          = TokenNew(lexer->input, lexer->pos);
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
  size_t start                = lexer->pos;
  size_t end                  = ConsumeLine(lexer);
  lexer->current_token.start  = start;
  lexer->current_token.end    = end;
  lexer->current_token.length = end - start;
  NewLine(lexer);
  CommitToken(lexer);
}

static void LexParagraph(Lexer *lexer) {
  lexer->current_token.type   = TOKEN_P;
  size_t start                = lexer->pos;
  size_t end                  = ConsumeLine(lexer);
  lexer->current_token.start  = start;
  lexer->current_token.end    = end;
  lexer->current_token.length = end - start;
  NewLine(lexer);
  CommitToken(lexer);
}

TokenStream *Lex(char *input) {
  Lexer lexer = LexerNew(input, 0, strlen(input));

  char c;

  while ((c = Peek(&lexer)) != '\0') {
    Whitespace(&lexer);
    switch (c) {
      case '#':
        LexHeading(&lexer);
        break;
      default:
        LexParagraph(&lexer);
        break;
    }
    Whitespace(&lexer);
    NewLine(&lexer);
  }

  return lexer.stream;
}

static void LexText(Lexer *lexer) {
  lexer->current_token.type   = TOKEN_TEXT;
  size_t start                = lexer->pos;
  size_t end                  = ConsumeUntilAny(lexer, "[*_\n", false);
  lexer->current_token.start  = start;
  lexer->current_token.end    = end;
  lexer->current_token.length = end - start;
  NewLine(lexer);
  CommitToken(lexer);
}

static void LexLink(Lexer *lexer) {
  lexer->current_token.type   = TOKEN_LINK;
  size_t start                = lexer->pos;
  size_t end                  = ConsumeUntil(lexer, ')', true);
  lexer->current_token.start  = start;
  lexer->current_token.end    = end;
  lexer->current_token.length = end - start;
  NewLine(lexer);
  CommitToken(lexer);
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

// TODO Handle mixed formats (* and _).
static void LexEmphasis(Lexer *lexer) {
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

  lexer->current_token.type   = type;
  lexer->current_token.start  = start;
  lexer->current_token.end    = end;
  lexer->current_token.length = end - start;

  CommitToken(lexer);
}

TokenStream *LexInline(Token *token) {
  Lexer lexer = LexerNew(token->input, token->start, token->end);

  char c;
  while ((c = Peek(&lexer)) != '\0' && c != '\n') {
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