#ifndef LEX_H
#define LEX_H

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

typedef enum {
  TOKEN_UNKNOWN,
  TOKEN_NULL,
  TOKEN_H1,
  TOKEN_H2,
  TOKEN_H3,
  TOKEN_H4,
  TOKEN_H5,
  TOKEN_H6,
  TOKEN_H1_COLLAPSING,
  TOKEN_H2_COLLAPSING,
  TOKEN_H3_COLLAPSING,
  TOKEN_H4_COLLAPSING,
  TOKEN_H5_COLLAPSING,
  TOKEN_H6_COLLAPSING,
  TOKEN_P,
  TOKEN_TEXT,
  TOKEN_LINK,
  TOKEN_LINKLABEL,
  TOKEN_LINKHREF,
  TOKEN_ITALICS,
  TOKEN_BOLD,
  TOKEN_BR,
  TOKEN_QUOTE,
  TOKEN_LISTITEMORDERED,
  TOKEN_LISTITEMUNORDERED,
  TOKEN_NESTED,
  TOKEN_EMPTYLINE,
  TOKEN_CODEBLOCK,
  TOKEN_CODEBLOCKINLINE,
  TOKEN_HR,
  TOKEN_ADMONITION,
  TOKEN_HTML,
  TOKEN_TABLEROW,
  TOKEN_TABLECELL,
  TOKEN_TABLESEPARATOR,
  TOKEN_ATTRIBUTES
} TokenType;

typedef struct {
  char *input;
  TokenType type;
  size_t start;
  size_t end;
  size_t length;
  size_t line;
  int indent_level;
} Token;

typedef struct Lexer {
  char *input;
  size_t pos;
  size_t end;
  size_t current_line;
} Lexer;

Lexer LexerNew(char *input, size_t start, size_t end);
Token NextToken(Lexer *lexer);
Token PeekToken(Lexer *lexer);
Token NextInlineToken(Lexer *lexer);
void TokenPrint(Token *token);

#endif  // LEX_H