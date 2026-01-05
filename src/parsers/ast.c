#include "ast.h"

Node* NewNode(const char* type) {
  Node* node = malloc(sizeof(Node));
  if (node == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  node->attributes       = (HTMLAttribute*)malloc(10 * sizeof(HTMLAttribute));
  node->attributes_count = 0;
  node->max_attributes   = 10;
  node->type             = type;
  node->first_child      = NULL;
  node->next_sibling     = NULL;
  node->input            = NULL;
  node->start            = 0;
  node->end              = 0;
  node->indent_level     = 0;
  node->is_inline        = false;
  return node;
}

void FreeNode(Node* node) {
  static int total_attrs_freed = 0;
  if (!node) return;

  if (node->first_child) FreeNode(node->first_child);
  if (node->next_sibling) FreeNode(node->next_sibling);

  for (int i = 0; i < node->attributes_count; i++) {
    free(node->attributes[i].name);
    free(node->attributes[i].value);
    total_attrs_freed++;
  }

  free(node->attributes);
  free(node);
}

// TODO Error handling
// TODO Test if the realloc works when enough attrs are added
Node* NodeAddAttribute(Node* node, char* name, char* value) {
  static int total_attr_count = 0;
  if (node->attributes_count + 1 >= node->max_attributes) {
    int new_max = node->max_attributes * 2;
    HTMLAttribute* new_attrs =
        realloc(node->attributes, new_max * sizeof(HTMLAttribute));
    node->attributes = new_attrs;

    if (new_attrs == NULL) {
      perror("realloc failed");
      exit(EXIT_FAILURE);
    }
    node->max_attributes = new_max;
  }

  node->attributes[node->attributes_count] =
      (HTMLAttribute){.name = malloc(1000), .value = malloc(1000)};
  strcpy(node->attributes[node->attributes_count].name, name);
  strcpy(node->attributes[node->attributes_count].value, value);
  node->attributes_count++;
  total_attr_count++;

  return node;
}

Node* NodeAppendSibling(Node* node, Node* sibling) {
  if (node->next_sibling == NULL) {
    node->next_sibling = sibling;
    return sibling;
  }

  return NodeAppendSibling(node->next_sibling, sibling);
}

Node* NodeAppendChild(Node* node, Node* child) {
  if (node->first_child == NULL) {
    node->first_child = child;
    return child;
  }

  return NodeAppendSibling(node->first_child, child);
}

Node* NodeFromToken(const char* type, Token* token) {
  Node* node         = NewNode(type);
  node->start        = token->start;
  node->end          = token->end;
  node->input        = token->input;
  node->indent_level = token->indent_level;
  return node;
}
