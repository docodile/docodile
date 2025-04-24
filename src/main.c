#include <stdio.h>

#include "html.h"
#include "lex.h"
#include "logger.h"

int main(void) {
  FILE *file = fopen("test/test.md", "r");

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* buffer = malloc(length + 1);
  fread(buffer, 1, length, file);
  buffer[length] = '\0';

  TokenStream *token_stream = Lex(buffer);

  RenderHtml(token_stream);

  fclose(file);

  return 0;
}