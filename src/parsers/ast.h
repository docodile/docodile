#ifndef AST_H
#define AST_H

#include "../utils.h"
#include "token.h"

typedef struct {
  char* name;
  char* value;
} HTMLAttribute;

typedef struct Node {
  const char* type;
  struct Node* first_child;
  struct Node* next_sibling;

  char* input;

  int start;
  int end;

  int indent_level;
  bool is_inline;

  int attributes_count;
  int max_attributes;
  HTMLAttribute* attributes;
} Node;

Node* NewNode(const char* type);
void FreeNode(Node* node);
Node* NodeAppendSibling(Node* node, Node* sibling);
Node* NodeAppendChild(Node* node, Node* child);
Node* NodeAddAttribute(Node* node, char* name, char* value);
Node* NodeFromToken(const char* type, Token* token);

#endif
