#include "parser.h"

Node *ParseInlineCode(Token *token);

Node *NewNode(NodeType type) {
  Node *node = (Node *)malloc(sizeof(Node));
  if (node == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  node->attributes       = (HTMLAttribute *)malloc(10 * sizeof(HTMLAttribute));
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

void FreeNode(Node *node) {
  if (node->first_child) {
    FreeNode(node->first_child);
  }

  if (node->next_sibling) {
    FreeNode(node->next_sibling);
  }

  free(node->attributes);
  free(node);
}

// TODO Error handling
// TODO Test if the realloc works when enough attrs are added
Node *NodeAddAttribute(Node *node, char *name, char *value) {
  if (node->attributes_count + 1 >= node->max_attributes) {
    int new_max = node->max_attributes * 2;
    HTMLAttribute *new_attrs =
        realloc(node->attributes, new_max * sizeof(HTMLAttribute));
    node->attributes = new_attrs;

    if (new_attrs == NULL) {
      perror("realloc failed");
      exit(EXIT_FAILURE);
    }
    node->max_attributes = new_max;
  }

  node->attributes[node->attributes_count] =
      (HTMLAttribute){.name = name, .value = value};
  node->attributes_count++;

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

Node *NodeAppendSibling(Node *node, Node *sibling) {
  if (node->next_sibling == NULL) {
    node->next_sibling = sibling;
    return sibling;
  }

  return NodeAppendSibling(node->next_sibling, sibling);
}

Node *NodeAppendChild(Node *node, Node *child) {
  if (node->first_child == NULL) {
    node->first_child = child;
    return child;
  }

  return NodeAppendSibling(node->first_child, child);
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

Node *ParseCodeBlock(Token *token) {
  Node *n = NodeFromToken(NODE_CODE, token);

  // HACK Get the code block language. This is not very readable and shouldn't
  // really be done this way. The lexer should most likely have an opening code
  // block token. But as a hack I've just made it so that it inline lexes the
  // language token and writes that to a node attribute. The start of the node
  // needs to be updated by the length of the language token so that it's not
  // included in the final output.

  Lexer inline_lexer = LexerNew(token->input, token->start, token->end);
  Token inline_token = NextInlineToken(&inline_lexer);
  if (inline_token.type == TOKEN_TEXT && token->length > 0) {
    char *buffer = malloc(100);
    int written  = snprintf(buffer, inline_token.length + 10, "language-%.*s",
                            inline_token.length + 1,
                            &inline_token.input[inline_token.start]);
    written += 9;  // "language-" = 9 chars
    buffer[written] = '\0';
    n->start        = n->start + inline_token.length + 1;

    NodeAddAttribute(n, "class", buffer);
  }

  return n;
}

Node *Parse(Lexer *lexer, Node *parent) {
  Node *doc   = NewNode(NODE_DOCUMENT);
  Token token = NextToken(lexer);

  Node *node;

  while (token.type != TOKEN_NULL) {
    bool skip = false;

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
      case TOKEN_CODEBLOCK:
        node = ParseCodeBlock(&token);
        break;
      case TOKEN_P:
      default:
        node = ParseParagraph(&token);
        break;
    }

    if (!skip) {
      NodeAppendChild(parent, node);
      // HACK Maybe not the nicest way of handling code blocks.
      if (token.type != TOKEN_CODEBLOCK) ParseInline(lexer, node);
    }

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

Node *ParseInlineCode(Token *token) {
  Node *n = NodeFromToken(NODE_INLINECODE, token);
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
      case TOKEN_CODEBLOCKINLINE:
        node = ParseInlineCode(&token);
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