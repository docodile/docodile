#ifndef PARSER_H
#define PARSER_H

#include "lex.h"

typedef struct {
  const char *input;
} Parser;

typedef enum {
  NODE_UNKNOWN,
  NODE_DOCUMENT,
  NODE_PARAGRAPH,
  NODE_LIST,
  NODE_LISTITEM,
  NODE_HEADING,
  NODE_QUOTE,
  NODE_EMPHASIS,
  NODE_LINK,
  NODE_TEXT,
  NODE_BREAK,
} NodeType;

typedef union {
  struct {
    int level;
  } Heading;

  struct {
  } Paragraph;

  struct {
    bool ordered;
  } List;

  struct {
    bool strong;
  } Emphasis;

  struct {
    size_t href_start;
    size_t href_end;
    size_t label_start;
    size_t label_end;
  } Link;
} NodeData;

typedef struct Node {
  NodeType type;
  struct Node *first_child;
  struct Node *next_sibling;

  char *input;

  int start;
  int end;

  int indent_level;

  NodeData data;
} Node;

Node *Parse(Lexer *lexer, Node *parent);
Node *ParseInline(Lexer *lexer, Node *parent);
Node *NewNode(NodeType type);

#endif  // PARSER_H