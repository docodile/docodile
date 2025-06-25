#include "search.h"

static void BuildIndex(SearchIndex *index, Directory *dir) {
  if (dir->hidden) return;

  if (!dir->is_dir) {
    if (index->doc_count >= index->max_docs) {
      index->max_docs *= 2;
      index->docs =
          realloc(index->docs, index->max_docs * sizeof(*index->docs));
      if (!index->docs) {
        perror("realloc");
        exit(1);
      }
    }

    char *name = malloc(strlen(dir->title) + 1);
    strcpy(name, dir->title);
    char *path = malloc(strlen(dir->url_path) + 1);
    strcpy(path, dir->url_path);
    size_t len;
    char *text    = ReadFileToString(dir->full_path, &len);
    char *escaped = malloc(len * 2);
    EscapeString(text, escaped);
    free(text);

    SearchDoc *doc = malloc(sizeof(SearchDoc));
    doc->name      = name;
    doc->path      = path;
    doc->text      = escaped;

    index->docs[index->doc_count++] = doc;

    return;
  }

  for (size_t i = 0; i < dir->num_dirs; i++) {
    if (dir->dirs[i]->path[0] == '_') continue;
    if (dir->dirs[i]->hidden) continue;

    BuildIndex(index, dir->dirs[i]);
  }
}

SearchIndex BuildSearchIndex(Directory *site_directory) {
  SearchIndex index = {.doc_count = 0,
                       .max_docs  = 100,
                       .docs      = malloc(100 * sizeof(SearchDoc *))};

  BuildIndex(&index, site_directory);

  return index;
}

void FreeSearchIndex(SearchIndex index) {
  if (!index.docs) return;

  for (size_t i = 0; i < index.doc_count; i++) {
    SearchDoc *doc = index.docs[i];

    if (!doc) continue;

    if (doc->name) free(doc->name);
    if (doc->path) free(doc->path);
    if (doc->text) free(doc->text);

    free(doc);
  }

  free(index.docs);
}

void WriteSearchIndex(SearchIndex index, const char *filename) {
  FILE *f = fopen(filename, "w");
  if (!f) {
    perror("fopen");
    exit(1);
  }

  fputc('[', f);

  for (size_t i = 0; i < index.doc_count; i++) {
    fputc('{', f);

    SearchDoc *doc = index.docs[i];

    if (doc->name) {
      fputs("\"name\":\"", f);
      fputs(doc->name, f);
      fputs("\",", f);
    }

    fputs("\"path\":\"", f);
    if (doc->path) {
      fputs(doc->path, f);
    }
    fputs("\",", f);

    fputs("\"text\":\"", f);
    if (doc->text) {
      fputs(doc->text, f);
    }
    fputs("\"", f);

    fputc('}', f);
    if (i < index.doc_count - 1) {
      fputc(',', f);
    }
  }

  fputc(']', f);
  fclose(f);
}