#include <stdio.h>

#include "htmlgenerator.h"
#include "lex.h"

int main(void) {
  FILE *f = fopen("test/test.md", "r");

  TokenStream *token_stream = Lex(f);

  GenerateHtml(token_stream);

  return 0;
}