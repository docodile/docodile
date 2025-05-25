#include "directory.h"

Page *NewPage(const char *name, const char *fullpath) {
  Page *page = malloc(sizeof(Page));
  ChangeFilePathExtension(".md", ".html", name, page->out_name);
  strcpy(page->src_name, name);
  strcpy(page->full_path, fullpath);
  return page;
}

static void FreePage(Page *page) { free(page); }

Directory *NewDirectory(const char *path) {
  Directory *dir = malloc(sizeof(Directory));
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