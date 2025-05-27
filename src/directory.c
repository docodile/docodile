#include "directory.h"

Page *NewPage(const char *name, const char *fullpath) {
  Page *page = malloc(sizeof(Page));
  ChangeFilePathExtension(".md", ".html", name, page->out_name);
  ChangeFilePathExtension(".md", ".html", fullpath, page->url_path);
  RemoveExtension(name, page->title);
  KebabCaseToTitleCase(page->title, page->title);
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