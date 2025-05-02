#include "parser.h"

Node *NewNode(NodeType type) {
  Node *node         = (Node *)malloc(sizeof(Node));
  node->type         = type;
  node->first_child  = NULL;
  node->next_sibling = NULL;
  node->input        = NULL;
  node->start        = 0;
  node->end          = 0;
  node->indent_level = 0;
  node->is_inline    = false;
  return node;
}

Node *NodeFromToken(NodeType type, Token *token) {
  Node *node         = NewNode(type);
  node->start        = token->start;
  node->end          = token->end;
  node->input        = token->input;
  node->indent_level = token->indent_level;
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

Node *ParseList(Token *token, Lexer *lexer) {
  Node *n              = NewNode(NODE_LIST);
  n->indent_level      = token->indent_level;
  n->data.List.ordered = token->type == TOKEN_LISTITEMORDERED;

  Node *first_item = NodeFromToken(NODE_LISTITEM, token);
  NodeAppendChild(n, first_item);
  ParseInline(lexer, first_item);

  Token next = PeekToken(lexer);
  while ((next.type == TOKEN_LISTITEMORDERED ||
          next.type == TOKEN_LISTITEMUNORDERED) &&
         next.indent_level >= n->indent_level) {
    next = NextToken(lexer);
    if (next.indent_level > n->indent_level) {
      NodeAppendChild(n, ParseList(&next, lexer));
    } else {
      Node *list_item = NodeFromToken(NODE_LISTITEM, &next);
      ParseInline(lexer, list_item);
      if (list_item->indent_level < n->indent_level) {
        NodeAppendSibling(n, list_item);
      } else {
        NodeAppendChild(n, list_item);
      }
    }
    next = PeekToken(lexer);
  }

  Token after = PeekToken(lexer);

  return n;
}

Node *Parse(Lexer *lexer, Node *parent) {
  Node *doc   = NewNode(NODE_DOCUMENT);
  Token token = NextToken(lexer);

  Node *node;

  bool skip = false;

  while (token.type != TOKEN_NULL) {
    switch (token.type) {
      case TOKEN_EMPTYLINE:
        skip = true;
        break;
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
      case TOKEN_LISTITEMORDERED:
      case TOKEN_LISTITEMUNORDERED:
        node = ParseList(&token, lexer);
        break;
      case TOKEN_P:
      default:
        node = ParseParagraph(&token);
        break;
    }

    if (!skip) {
      NodeAppendChild(parent, node);
      ParseInline(lexer, node);
    }

    skip  = false;
    token = NextToken(lexer);
  }

  return doc;
}

Node *ParseListItem(Token *token) {
  Node *n = NodeFromToken(NODE_LISTITEM, token);
  // TODO Calculate indent_level.
  return n;
}

Node *ParseLink(Token *token, Lexer *lexer) {
  Node *n                  = NodeFromToken(NODE_LINK, token);
  n->data.Link.label_start = token->start;
  n->data.Link.label_end   = token->end;

  Token link_href_token = NextInlineToken(lexer);

  // TODO Silently fail and fallback to text rendering.
  assert(link_href_token.type == TOKEN_LINKHREF);

  n->data.Link.href_start = link_href_token.start;
  n->data.Link.href_end   = link_href_token.end;

  return n;
}

Node *ParseBreak(Token *token) {
  Node *n = NodeFromToken(NODE_BREAK, token);
  return n;
}

Node *ParseEmphasis(Token *token, bool strong) {
  Node *n                 = NodeFromToken(NODE_EMPHASIS, token);
  n->data.Emphasis.strong = strong;
  return n;
}

Node *ParseText(Token *token) {
  Node *n = NodeFromToken(NODE_TEXT, token);
  return n;
}

Node *ParseInline(Lexer *lexer, Node *parent) {
  Lexer inline_lexer = LexerNew(lexer->input, parent->start, parent->end);
  Token token        = NextInlineToken(&inline_lexer);
  Node *node;

  while (token.type != TOKEN_NULL) {
    switch (token.type) {
      case TOKEN_ITALICS:
        node = ParseEmphasis(&token, false);
        break;
      case TOKEN_BOLD:
        node = ParseEmphasis(&token, true);
        break;
      case TOKEN_LINKLABEL:
        node = ParseLink(&token, &inline_lexer);
        break;
      case TOKEN_BR:
        node = ParseBreak(&token);
        break;
      case TOKEN_TEXT:
      default:
        node = ParseText(&token);
        break;
    }

    node->is_inline = true;
    NodeAppendChild(parent, node);

    // TODO Determine exhaustive list of terminals.
    if (node->type != NODE_TEXT && node->type != NODE_LINK &&
        node->type != NODE_BREAK) {
      ParseInline(&inline_lexer, node);
    }

    token = NextInlineToken(&inline_lexer);
  }
}