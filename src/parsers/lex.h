#ifndef LEX_H
#define LEX_H

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

typedef struct Lexer {
  char* input;
  size_t pos;
  size_t end;
  size_t current_line;
} Lexer;

Lexer LexerNew(char* input, size_t start, size_t end);
Token NextToken(Lexer* lexer);
Token PeekToken(Lexer* lexer);
Token NextInlineToken(Lexer* lexer);

#endif  // LEX_H
