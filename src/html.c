#include "html.h"

#include <stdarg.h>

static FILE *out;

#define print(...) fprintf(out, __VA_ARGS__)

static char *RenderAttributes(Node *node) {
  if (node->attributes_count == 0) return NULL;

  size_t required_mem = 0;
  for (int i = 0; i < node->attributes_count; i++)
    required_mem += strlen(node->attributes[i].name) +
                    strlen(node->attributes[i].value) + 3;
  required_mem++;
  size_t written = 0;
  char *buffer   = malloc(required_mem);
  for (int i = 0; i < node->attributes_count; i++) {
    HTMLAttribute attr = node->attributes[i];
    if (strcmp("_attrs", attr.name) == 0) {
      written += sprintf(&buffer[written], "%s", attr.value);
    } else {
      written += sprintf(&buffer[written], "%s=\"%s\"", attr.name, attr.value);
    }
  }

  return buffer;
}

static void RenderNodeTag(Node *node, bool closing) {
  if (strcmp("_text", node->type) == 0) return;
  print("<");
  if (closing) print("/");
  print("%s", node->type);
  if (!closing) {
    char *attrs = RenderAttributes(node);
    if (attrs) {
      print(" %s", attrs);
      free(attrs);
    }
  }
  print(">");
}

void RenderNodeContent(Node *node) {
  if (strcmp("a", node->type) == 0 || strcmp("_text", node->type) == 0 ||
      strcmp("code", node->type) == 0 || strcmp("_html", node->type) == 0) {
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

  if (strcmp("_attrs", node->type) == 0) {
    return;
  }

  if (strcmp("_html", node->type) == 0) {
    RenderNodeContent(node);
    RenderNode(node->next_sibling, indent, false);
    return;
  }

  if (strcmp("br", node->type) == 0) {
    print("<br>");
    RenderNode(node->next_sibling, indent, false);
    return;
  }

  if (strcmp("hr", node->type) == 0) {
    print("<hr>");
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