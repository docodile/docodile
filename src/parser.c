#include "parser.h"

Node *ParseInlineCode(Token *token);

Node *NewNode(const char *type) {
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

  for (size_t i = 0; i < node->attributes_count; i++) {
    free(node->attributes[i].value);
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

static Node *NodeFromToken(const char *type, Token *token) {
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
  const char *heading = "h1";
  switch (level) {
    case 6:
      heading = "h6";
      break;
    case 5:
      heading = "h5";
      break;
    case 4:
      heading = "h4";
      break;
    case 3:
      heading = "h3";
      break;
    case 2:
      heading = "h2";
      break;
    case 1:
    default:
      heading = "h1";
      break;
  }
  Node *n    = NodeFromToken(heading, token);
  size_t len = n->end - n->start;
  char *id   = malloc(len + 1);
  snprintf(id, len + 1, "%s", &n->input[n->start]);
  TitleCaseToKebabCase(id, id);
  id[len] = '\0';
  NodeAddAttribute(n, "id", id);
  return n;
}

Node *ParseParagraph(Token *token) {
  Node *n = NodeFromToken("p", token);
  return n;
}

Node *ParseQuote(Token *token) {
  Node *n = NodeFromToken("blockquote", token);
  return n;
}

Node *ParseHorizontalRule(Token *token) {
  Node *n = NodeFromToken("hr", token);
  return n;
}

Node *ParseList(Token *token, Lexer *lexer) {
  Node *n         = NewNode(token->type == TOKEN_LISTITEMORDERED ? "ol" : "ul");
  n->indent_level = token->indent_level;

  Node *first_item = NodeFromToken("li", token);
  NodeAppendChild(n, first_item);
  ParseInline(lexer, first_item);

  Token next    = PeekToken(lexer);
  Node *current = first_item;
  while ((next.type == TOKEN_LISTITEMORDERED ||
          next.type == TOKEN_LISTITEMUNORDERED) &&
         next.indent_level >= n->indent_level) {
    next = NextToken(lexer);
    if (next.indent_level > n->indent_level) {
      NodeAppendChild(current, ParseList(&next, lexer));
    } else {
      Node *list_item = NodeFromToken("li", &next);
      current         = list_item;
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
  Node *n = NodeFromToken("code", token);

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

  Node *pre = NodeFromToken("pre", token);
  NodeAppendChild(pre, n);

  return pre;
}

Node *Parse(Lexer *lexer, Node *parent) {
  Node *doc   = NewNode("article");
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
      case TOKEN_HR:
        node = ParseHorizontalRule(&token);
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
  Node *n = NodeFromToken("li", token);
  // TODO Calculate indent_level.
  return n;
}

Node *ParseLink(Token *token, Lexer *lexer) {
  Node *n               = NodeFromToken("a", token);
  Token link_href_token = NextInlineToken(lexer);

  // TODO Silently fail and fallback to text rendering.
  assert(link_href_token.type == TOKEN_LINKHREF);

  char *href_value = malloc(1000);
  sprintf(href_value, "%.*s", link_href_token.end - link_href_token.start,
          &n->input[link_href_token.start]);
  char *href  = strtok(href_value, " ");
  char *title = strtok(NULL, "");
  ChangeFilePathExtension(".md", ".html", href, href);
  NodeAddAttribute(n, "href", href);

  if (title) {
    if (title[0] == '"') title++;
    char *buffer = malloc(strlen(title));
    strcpy(buffer, title);
    size_t len = strlen(buffer);
    if (buffer[len - 1] == '"') buffer[len - 1] = '\0';
    NodeAddAttribute(n, "title", buffer);
  }

  return n;
}

Node *ParseBreak(Token *token) {
  Node *n = NodeFromToken("br", token);
  return n;
}

Node *ParseInlineCode(Token *token) {
  Node *n = NodeFromToken("code", token);
  return n;
}

Node *ParseEmphasis(Token *token, bool strong) {
  Node *n = NodeFromToken(strong ? "strong" : "em", token);
  return n;
}

Node *ParseText(Token *token) {
  Node *n = NodeFromToken("_text", token);
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
    if (strcmp("_text", node->type) != 0 && strcmp("a", node->type) != 0 &&
        strcmp("br", node->type) != 0 && strcmp("code", node->type) != 0) {
      ParseInline(&inline_lexer, node);
    }

    token = NextInlineToken(&inline_lexer);
  }
}