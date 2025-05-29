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
  Page *page = malloc(sizeof(Page));
  ChangeFilePathExtension(".md", ".html", name, page->out_name);
  GenerateUrl(fullpath, page->url_path);
  GenerateTitle(fullpath, page->title);
  strcpy(page->src_name, name);
  strcpy(page->full_path, fullpath);
  page->toc = (TOC){.count = 0, .items = malloc(sizeof(char *))};
  return page;
}

static void FreePage(Page *page) {
  for (size_t i = 0; i < page->toc.count; i++) free(page->toc.items[i]);
  free(page->toc.items);
  free(page);
}

Directory *NewDirectory(const char *path) {
  Directory *dir = malloc(sizeof(Directory));
  KebabCaseToTitleCase(path, dir->title);
  strcpy(dir->path, path);
  dir->num_pages = 0;
  dir->num_dirs  = 0;
  dir->hidden    = false;
  return dir;
}

void FreeDirectory(Directory *dir) {
  for (size_t i = 0; i < dir->num_pages; i++) {
    FreePage(dir->pages[i]);
  }

  for (size_t i = 0; i < dir->num_dirs; i++) {
    FreeDirectory(dir->dirs[i]);
  }

  free(dir);
}