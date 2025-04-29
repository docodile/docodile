#include "parser.h"

Node *NewNode(NodeType type) {
  Node *node         = (Node *)malloc(sizeof(Node));
  node->type         = type;
  node->first_child  = NULL;
  node->next_sibling = NULL;
  node->start        = 0;
  node->end          = 0;
  node->indent_level = 0;
  return node;
}

Node *NodeFromToken(NodeType type, Token *token) {
  Node *node  = NewNode(type);
  node->start = token->start;
  node->end   = token->end;
  node->input = token->input;
  return node;
}

Node *NodeAppendSibling(Node *prev, Node *curr) {
  if (prev->next_sibling == NULL) {
    prev->next_sibling = curr;
    return curr;
  }

  return NodeAppendSibling(prev->next_sibling, curr);
}

Node *NodeAppendChild(Node *parent, Node *child) {
  if (parent->first_child == NULL) {
    parent->first_child = child;
    return child;
  }

  return NodeAppendSibling(parent->first_child, child);
}

void PrintNodeType(Node *node) {
  switch (node->type) {
    case NODE_UNKNOWN:
      printf("<UNKNOWN>");
      break;
    case NODE_DOCUMENT:
      printf("<document>");
      break;
    case NODE_PARAGRAPH:
      printf("<paragraph>");
      break;
    case NODE_LIST:
      printf("<list>");
      break;
    case NODE_LISTITEM:
      printf("<listitem>");
      break;
    case NODE_TEXT:
      printf("<text>");
      break;
    case NODE_HEADING:
      printf("<heading>");
      break;
    case NODE_LINK:
      printf("<link>");
      break;
    case NODE_QUOTE:
      printf("<quote>");
      break;
  }

  printf("\n");
}

void PrintNode(Node *node, int indent) {
  for (int i = 0; i < indent; i++) {
    if (i == 0) {
      printf("├─");
    } else {
      printf("──");
    }
  }

  PrintNodeType(node);

  if (node->first_child) {
    PrintNode(node->first_child, indent + 1);
  }

  if (node->next_sibling) {
    PrintNode(node->next_sibling, indent);
  }
}

Node *ParseHeading(Token *token, int level) {
  Node *n               = NodeFromToken(NODE_HEADING, token);
  n->data.Heading.level = level;
  return n;
}

Node *ParseParagraph(Token *token) {
  Node *n = NodeFromToken(NODE_PARAGRAPH, token);
  return n;
}

Node *ParseQuote(Token *token) {
  Node *n = NodeFromToken(NODE_QUOTE, token);
  return n;
}

Node *ParseOrderedList(Token *token) {
  Node *n              = NodeFromToken(NODE_LIST, token);
  n->data.List.ordered = true;
  return n;
}

Node *ParseUnorderedList(Token *token) {
  Node *n              = NodeFromToken(NODE_LIST, token);
  n->data.List.ordered = false;
  return n;
}

Node *Parse(Lexer *lexer, Node *parent) {
  Node *doc   = NewNode(NODE_DOCUMENT);
  Token token = NextToken(lexer);

  Node *node;

  while (token.type != TOKEN_NULL) {
    switch (token.type) {
      case TOKEN_H1:
        node = ParseHeading(&token, 1);
        break;
      case TOKEN_H2:
        node = ParseHeading(&token, 2);
        break;
      case TOKEN_H3:
        node = ParseHeading(&token, 3);
        break;
      case TOKEN_H4:
        node = ParseHeading(&token, 4);
        break;
      case TOKEN_H5:
        node = ParseHeading(&token, 5);
        break;
      case TOKEN_H6:
        node = ParseHeading(&token, 6);
        break;
      case TOKEN_QUOTE:
        node = ParseQuote(&token);
        break;
      case TOKEN_ORDEREDLIST:
        node = ParseOrderedList(&token);
        break;
      case TOKEN_UNORDEREDLIST:
        node = ParseUnorderedList(&token);
        break;
      case TOKEN_P:
      default:
        node = ParseParagraph(&token);
        break;
    }

    NodeAppendChild(parent, node);
    ParseInline(lexer, node);

    token = NextToken(lexer);
  }

  return doc;
}

Node *ParseListItem(Token *token) {
  Node *n = NodeFromToken(NODE_LISTITEM, token);
  // TODO Calculate indent_level.
  return n;
}

Node *ParseLink(Token *token) {
  Node *n = NodeFromToken(NODE_LINK, token);
  // TODO Get href and label. Maybe this should just be in the lexer?
  n->data.Link.href_start  = 0;
  n->data.Link.href_end    = 0;
  n->data.Link.label_start = 0;
  n->data.Link.label_end   = 0;

  return n;
}

Node *ParseBreak(Token *token) {
  Node *n = NodeFromToken(NODE_BREAK, token);
  return n;
}

Node *ParseText(Token *token, bool bold, bool italics) {
  Node *n               = NodeFromToken(NODE_TEXT, token);
  n->data.Text.emphasis = italics;
  n->data.Text.strong   = bold;
  return n;
}

Node *ParseInline(Lexer *lexer, Node *parent) {
  Lexer inline_lexer = LexerNew(lexer->input, parent->start, parent->end);
  Token token        = NextInlineToken(&inline_lexer);
  Node *node;

  while (token.type != TOKEN_NULL) {
    switch (token.type) {
      case TOKEN_LISTITEM:
        node = ParseListItem(&token);
        break;
      case TOKEN_ITALICS:
        node = ParseText(&token, false, true);
        break;
      case TOKEN_BOLD:
        node = ParseText(&token, true, false);
        break;
      case TOKEN_LINK:
        node = ParseLink(&token);
        break;
      case TOKEN_BR:
        node = ParseBreak(&token);
        break;
      case TOKEN_TEXT:
      default:
        node = ParseText(&token, false, false);
        break;
    }

    NodeAppendChild(parent, node);

    // TODO Determine exhaustive list of terminals.
    if (node->type != NODE_TEXT && node->type != NODE_LINK &&
        node->type != NODE_BREAK) {
      ParseInline(&inline_lexer, node);
    }

    token = NextInlineToken(&inline_lexer);
  }
}