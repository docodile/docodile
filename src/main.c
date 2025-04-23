#include <stdio.h>

#include "html.h"
#include "lex.h"

int main(void) {
  FILE *f = fopen("test/test.md", "r");

  TokenStream *token_stream = Lex(f);

  RenderHtml(token_stream);

  return 0;
}