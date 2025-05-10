#include <stdio.h>

#include "html.h"
#include "lex.h"
#include "logger.h"
#include "parser.h"

int main(void) {
  FILE *file = fopen("docs/index.md", "r");
  if (!file) {
    perror("Failed to open file");
    return 1;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Failed to seek to end");
    fclose(file);
    return 1;
  }

  long length = ftell(file);
  if (length < 0) {
    perror("Failed to tell file length");
    fclose(file);
    return 1;
  }

  if (fseek(file, 0, SEEK_SET) != 0) {
    perror("Failed to seek to start");
    fclose(file);
    return 1;
  }

  char *buffer = malloc(length + 1);
  if (!buffer) {
    perror("Failed to allocate memory");
    fclose(file);
    return 1;
  }

  size_t bytes_read = fread(buffer, 1, length, file);
  if (bytes_read != (size_t)length) {
    fprintf(stderr, "Failed to read file: expected %ld bytes, got %zu\n",
            length, bytes_read);
    free(buffer);
    fclose(file);
    return 1;
  }

  Lexer lexer = LexerNew(buffer, 0, length);

  Node *doc = NewNode(NODE_DOCUMENT);
  Parse(&lexer, doc);

  RenderHtml(doc);

  free(buffer);
  fclose(file);

  return 0;
}