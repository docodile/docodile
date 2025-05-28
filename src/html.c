#include "html.h"

#include <stdarg.h>

#define OPENTAG(name)      "<" name ">"
#define CLOSETAG(name)     "</" name ">"
#define TAG(name, closing) closing ? CLOSETAG(name) : OPENTAG(name)

static FILE *out;

#define print(...) fprintf(out, __VA_ARGS__)

static char *RenderAttributes(Node *node) {
  size_t buffer_size = node->attributes_count * 32;
  size_t written     = 0;
  char *buffer       = malloc(buffer_size);
  for (int i = 0; i < node->attributes_count; i++) {
    if (written >= buffer_size / 2) {
      buffer = realloc(buffer, (buffer_size *= 2));
    }

    HTMLAttribute attr = node->attributes[i];
    written += sprintf(buffer, "%s=\"%s\"", attr.name, attr.value);
  }

  buffer[written] = '\0';

  return buffer;
}

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

static void RenderCode(Node *node, bool closing) {
  char *attrs = RenderAttributes(node);
  if (!closing) print(TAG("pre", closing));
  print(TAG("code %s", closing), attrs);
  if (closing) print(TAG("pre", closing));
  free(attrs);
}

static void RenderNodeTag(Node *node, bool closing) {
  switch (node->type) {
    case NODE_DOCUMENT:
      print(TAG("article", closing));
      break;
    case NODE_HEADING:
      char title[100];
      size_t len = node->end - node->start;
      strncpy(title, &node->input[node->start], len);
      title[len] = '\0';
      char id[100];
      TitleCaseToKebabCase(title, id);
      print(TAG("h%d id=\"%s\"", closing), node->data.Heading.level, id);
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
      RenderCode(node, closing);
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