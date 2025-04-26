#include <stdio.h>

#include "html.h"
#include "lex.h"
#include "logger.h"

int main(void) {
  FILE *file = fopen("test/test.md", "r");

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = malloc(length + 1);
  fread(buffer, 1, length, file);
  buffer[length] = '\0';

  Lexer lexer = LexerNew(buffer, 0, length);

  RenderHtml(&lexer);

  free(buffer);
  fclose(file);

  return 0;
}