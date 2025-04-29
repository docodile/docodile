#include "html.h"

#define OPENTAG(name)      "<" name ">"
#define CLOSETAG(name)     "</" name ">"
#define TAG(name, closing) closing ? CLOSETAG(name) : OPENTAG(name)

static void RenderNodeTag(Node *node, bool closing) {
  switch (node->type) {
    case NODE_DOCUMENT:
      printf(TAG("article", closing));
      break;
    case NODE_HEADING:
      printf(TAG("h%d", closing), node->data.Heading.level);
      break;
    case NODE_LINK:
      printf(TAG("a", closing));  // TODO href
      break;
    case NODE_LIST:
      printf(TAG("%2s", closing), node->data.List.ordered ? "ol" : "ul");
      break;
    case NODE_LISTITEM:
      printf(TAG("li", closing));
      break;
    case NODE_PARAGRAPH:
      printf(TAG("p", closing));
      break;
    case NODE_QUOTE:
      printf(TAG("blockquote", closing));
      break;
    case NODE_EMPHASIS: {
      if (node->data.Emphasis.strong) {
        printf(TAG("strong", closing));
      } else {
        printf(TAG("em", closing));
      }
    } break;
  }
}

void RenderNodeContent(Node *node) {
  if (node->type == NODE_TEXT) {
    size_t length = node->end - node->start;
    assert(length >= 0);
    printf("%.*s", length, &node->input[node->start]);
  }
}

void RenderHtml(Node *node) {
  if (node == NULL) return;

  if (node->type == NODE_BREAK) {
    printf("<br />");
    RenderHtml(node->next_sibling);
    return;
  }

  RenderNodeTag(node, false);
  {
    RenderNodeContent(node);
    RenderHtml(node->first_child);
  }
  RenderNodeTag(node, true);

  RenderHtml(node->next_sibling);
}