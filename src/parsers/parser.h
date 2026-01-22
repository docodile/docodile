#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lex.h"
#include "utils.h"

typedef struct {
  const char *input;
} Parser;

Node *Parse(Lexer *lexer, Node *parent);

#endif  // PARSER_H
