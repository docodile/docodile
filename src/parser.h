#ifndef PARSER_H
#define PARSER_H

#include "lex.h"
#include "utils.h"

typedef struct {
  const char *input;
} Parser;

typedef struct {
  char *name;
  char *value;
} HTMLAttribute;

typedef struct Node {
  const char *type;
  struct Node *first_child;
  struct Node *next_sibling;

  char *input;

  int start;
  int end;

  int indent_level;
  bool is_inline;

  int attributes_count;
  int max_attributes;
  HTMLAttribute *attributes;
} Node;

Node *Parse(Lexer *lexer, Node *parent);
Node *ParseInline(Lexer *lexer, Node *parent);
Node *NewNode(const char *type);
void FreeNode(Node *node);
Node *NodeAppendSibling(Node *node, Node *sibling);
Node *NodeAppendChild(Node *node, Node *child);

#endif  // PARSER_H