#ifndef YAMLLEX_H
#define YAMLLEX_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TOKEN_KEY,
  TOKEN_VALUE,
  TOKEN_LISTITEM,
  TOKEN_LITERAL_STRING,
  TOKEN_LITERAL_NUMBER,
  TOKEN_LITERAL_BOOL,
} TokenType;

typedef struct {
  char *input;
  TokenType type;
  size_t start;
  size_t length;
  size_t indent_level;
} Token;

typedef struct {
  char *input;
  size_t pos;
  size_t end;
} Lexer;

#endif  // YAMLLEX_H