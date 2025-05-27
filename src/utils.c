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
  strcpy(out, title);
}