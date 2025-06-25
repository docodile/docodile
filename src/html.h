#ifndef HTML_H
#define HTML_H

#include <assert.h>

#include "parser.h"
#include "utils.h"

void RenderHtml(Node *ast, FILE *out_file);

#endif  // HTML_H