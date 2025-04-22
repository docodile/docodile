#include <stdio.h>

#include "lex.h"

int main(void) {
  FILE *f = fopen("test/test.md", "r");

  TokenStream *token_stream = Lex(f);

  return 0;
}