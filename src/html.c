#include "html.h"

#include <stdarg.h>

#define OPENTAG(name)      "<" name ">"
#define CLOSETAG(name)     "</" name ">"
#define TAG(name, closing) closing ? CLOSETAG(name) : OPENTAG(name)

static FILE *out;

#define print(...) fprintf(out, __VA_ARGS__)

static void RenderLink(Node *node, bool closing) {
  if (!closing) {
    size_t start  = node->data.Link.href_start;
    size_t end    = node->data.Link.href_end;
    size_t length = end - start;
    char raw_link[length];
    snprintf(raw_link, length + 1, "%s", &node->input[start]);
    char html_link[length + 2];
    ChangeFilePathExtension(".md", ".html", raw_link, html_link);
    print(TAG("a href=\"%.*s\"", closing), length + 2, html_link);
  } else {
    print(TAG("a", closing));
  }
}

static void RenderNodeTag(Node *node, bool closing) {
  switch (node->type) {
    case NODE_DOCUMENT:
      print(TAG("article", closing));
      break;
    case NODE_HEADING:
      print(TAG("h%d", closing), node->data.Heading.level);
      break;
    case NODE_LINK:
      RenderLink(node, closing);
      break;
    case NODE_LIST:
      print(TAG("%2s", closing), node->data.List.ordered ? "ol" : "ul");
      break;
    case NODE_LISTITEM:
      print(TAG("li", closing));
      break;
    case NODE_PARAGRAPH:
      print(TAG("p", closing));
      break;
    case NODE_QUOTE:
      print(TAG("blockquote", closing));
      break;
    case NODE_CODE:
      print(TAG("code", closing));
      break;
    case NODE_INLINECODE:
      print(TAG("code", closing));
      break;
    case NODE_EMPHASIS: {
      if (node->data.Emphasis.strong) {
        print(TAG("strong", closing));
      } else {
        print(TAG("em", closing));
      }
    } break;
  }
}

void RenderNodeContent(Node *node) {
  if (node->type == NODE_LINK) {
    size_t start  = node->data.Link.label_start;
    size_t length = node->data.Link.label_end - start;
    assert(length >= 0);
    print("%.*s", length, &node->input[start]);
  }

  if (node->type == NODE_TEXT || node->type == NODE_CODE) {
    size_t length = node->end - node->start;
    assert(length >= 0);
    print("%.*s", length, &node->input[node->start]);
  }
}

bool Break(Node *node) {
  bool should_break = true;

  if (node->is_inline) should_break = false;

  if (should_break) print("\n");

  return should_break;
}

void Indent(int indent) {
  for (int i = 0; i < indent; i++) {
    print("  ");
  }
}

static void RenderNode(Node *node, int indent, bool should_indent) {
  if (node == NULL) return;

  if (node->type == NODE_BREAK) {
    print("<br />");
    RenderNode(node->next_sibling, indent, false);
    return;
  }

  if (should_indent) Indent(indent);
  RenderNodeTag(node, false);
  int did_break = Break(node);
  {
    RenderNodeContent(node);
    RenderNode(node->first_child, indent + 1, did_break);
  }
  if (Break(node)) Indent(indent);
  RenderNodeTag(node, true);
  if (node->next_sibling) Break(node);

  RenderNode(node->next_sibling, indent, did_break);
}

void RenderHtml(Node *node, FILE *file) {
  out = file;
  RenderNode(node, 0, false);
}