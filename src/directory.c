#include "directory.h"

static void GenerateTitle(const char *in, char *out) {
  char buffer[1000];
  strcpy(buffer, in);

  char *first  = strtok(buffer, "/");
  char *second = strtok(NULL, "/");
  while (second != NULL) {
    char *next = strtok(NULL, "/");
    if (!next) break;
    first  = second;
    second = next;
  }

  if (strcmp("index.md", second) == 0)
    strcpy(buffer, first);
  else
    strcpy(buffer, second);

  RemoveExtension(buffer, out);
  KebabCaseToTitleCase(out, out);
}

static void GenerateUrl(const char *in, char *out) {
  char article_link[MAXURL];
  strcpy(article_link, in);
  strtok(article_link, "/");
  char *path = strtok(NULL, "");
  ChangeFilePathExtension(".md", ".html", path, out);
  sprintf(article_link, "/%s", out);
  strcpy(out, article_link);
}

Page *NewPage(const char *name, const char *fullpath) {
  Page *page     = malloc(sizeof(Page));
  page->is_index = strcmp("index.md", name) == 0;
  ChangeFilePathExtension(".md", ".html", name, page->out_name);
  GenerateUrl(fullpath, page->url_path);
  GenerateTitle(fullpath, page->title);
  strcpy(page->src_name, name);
  strcpy(page->full_path, fullpath);
  strcpy(page->path, fullpath);
  page->is_dir   = false;
  page->num_dirs = 0;
  page->toc      = (TOC){.count = 0, .items = malloc(sizeof(char *))};
  return page;
}

Directory *NewDirectory(const char *path) {
  Directory *dir = malloc(sizeof(Directory));
  KebabCaseToTitleCase(path, dir->title);
  dir->is_index = false;
  strcpy(dir->path, path);
  dir->num_dirs = 0;
  dir->hidden   = false;
  dir->is_dir   = true;
  dir->toc      = (TOC){.count = 0, .items = malloc(sizeof(char *))};
  return dir;
}

static int CompareDirs(const void *a, const void *b) {
  Directory *dir_a = *(Directory **)a;
  Directory *dir_b = *(Directory **)b;
  int cmp          = 0;
  if (dir_a->is_index && !dir_b->is_index)
    cmp = -1;
  else if (!dir_a->is_index && dir_b->is_index)
    cmp = 1;
  else
    cmp = strcasecmp(dir_a->title, dir_b->title);

  return cmp;
}

void SortDirectory(Directory *dir) {
  qsort(dir->dirs, dir->num_dirs, sizeof(Directory *), CompareDirs);
  for (int i = 0; i < dir->num_dirs; i++) {
    SortDirectory(dir->dirs[i]);
  }
}

void FreeDirectory(Directory *dir) {
  for (size_t i = 0; i < dir->num_dirs; i++) {
    FreeDirectory(dir->dirs[i]);
  }

  for (size_t i = 0; i < dir->toc.count; i++) free(dir->toc.items[i].link);
  free(dir->toc.items);

  free(dir);
}