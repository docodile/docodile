#ifndef SEARCH_H
#define SEARCH_H

#include <stdio.h>

#include "directory.h"

typedef struct {
  char *name;
  char *text;
  char *path;
} SearchDoc;

typedef struct {
  size_t doc_count;
  size_t max_docs;
  SearchDoc **docs;
} SearchIndex;

SearchIndex BuildSearchIndex(Directory *site_directory);
void WriteSearchIndex(SearchIndex index, const char *filename);
void FreeSearchIndex(SearchIndex index);

#endif  // SEARCH_H