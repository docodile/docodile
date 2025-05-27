#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void ChangeFilePathExtension(const char *from, const char *to, const char *in,
                             char *out);
void KebabCaseToTitleCase(const char *in, char *out);
void RemoveExtension(const char *in, char *out);

#endif  // UTILS_H