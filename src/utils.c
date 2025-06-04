#include "utils.h"

void ChangeFilePathExtension(const char *from, const char *to, const char *in,
                             char *out) {
  const char *ext = strrchr(in, '.');
  if (ext && strcmp(ext, from) == 0) {
    size_t base_len = ext - in;
    strncpy(out, in, base_len);
    strcpy(out + base_len, to);
  } else {
    strcpy(out, in);
  }
}

void KebabCaseToTitleCase(const char *in, char *out) {
  char c;
  int up = 1;
  while ((c = *in++) != '\0') {
    if (c == '-') {
      *out++ = ' ';
      up     = 1;
      continue;
    }

    *out++ = up ? (char)toupper(c) : c;
    up     = 0;
  }

  *out = '\0';
}

void TitleCaseToKebabCase(const char *in, char *out) {
  char c;
  while ((c = *in++) != '\0') {
    if (c == ' ') {
      *out++ = '-';
      continue;
    }

    *out++ = (char)tolower(c);
  }

  *out = '\0';
}

void RemoveExtension(const char *in, char *out) {
  char with_ext[1000];
  strcpy(with_ext, in);
  char *title = strtok(with_ext, ".");
  if (!title) return;
  strcpy(out, title);
}

char *ReadFileToString(const char *filename, size_t *len) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Couldn't find file");
    exit(1);
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Failed to seek to end");
    fclose(file);
    exit(1);
  }

  long length = ftell(file);
  if (length < 0) {
    perror("Failed to tell file length");
    fclose(file);
    exit(1);
  }

  if (fseek(file, 0, SEEK_SET) != 0) {
    perror("Failed to seek to start");
    fclose(file);
    exit(1);
  }

  char *buffer = malloc(length + 1);
  if (!buffer) {
    perror("Failed to allocate memory");
    fclose(file);
    exit(1);
  }

  size_t read_bytes = fread(buffer, 1, length, file);
  if (read_bytes != (size_t)length) {
    perror("Failed to read full file");
    free(buffer);
    fclose(file);
    exit(1);
  }

  buffer[length] = '\0';
  *len           = length;

  fclose(file);

  return buffer;
}