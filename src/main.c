#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "html.h"
#include "lex.h"
#include "logger.h"
#include "parser.h"
#include "server.h"
#include "template.h"

#define BUILDDIR       "site"
#define DOCSDIR        "docs"
#define DOCSMAXDEPTH   1000
#define MAXDIRS        1000
#define MAXPAGESPERDIR 1000
#define MAXFILENAMELEN 1000
#define MAXFILEPATH    1000

typedef struct {
  char out_name[MAXFILENAMELEN];
  char src_name[MAXFILENAMELEN];
  char full_path[MAXFILEPATH];
} Page;

static Page *NewPage(const char *name, const char *fullpath) {
  Page *page = malloc(sizeof(Page));
  ChangeFilePathExtension(".md", ".html", name, page->out_name);
  strcpy(page->src_name, name);
  strcpy(page->full_path, fullpath);
  return page;
}

typedef struct Directory {
  char path[MAXFILEPATH];
  bool hidden;
  size_t num_pages;
  size_t num_dirs;
  Page *pages[MAXPAGESPERDIR];
  struct Directory *dirs[MAXDIRS];
} Directory;

// TODO FreeDirectory function
static Directory *NewDirectory(const char *path) {
  Directory *dir = malloc(sizeof(Directory));
  strcpy(dir->path, path);
  dir->num_pages = 0;
  dir->num_dirs  = 0;
  dir->hidden    = false;
  return dir;
}

static void AddPage(Directory *dir, Page *page) {
  assert(dir->num_pages + 1 < MAXPAGESPERDIR);
  dir->pages[dir->num_pages++] = page;
}

static void AddDirectory(Directory *dir, Directory *child) {
  assert(dir->num_dirs + 1 < MAXPAGESPERDIR);
  dir->dirs[dir->num_dirs++] = child;
}

void BuildSiteDirectory(Directory *dest, const char *path) {
  DIR *dir = opendir(path);
  if (!dir) {
    perror("opendir");
    return;
  }

  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char fullpath[DOCSMAXDEPTH];
    sprintf(fullpath, "%s/%s", path, entry->d_name);

    struct stat statbuf;
    if (stat(fullpath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
      Directory *child_dir = NewDirectory(entry->d_name);
      AddDirectory(dest, child_dir);
      BuildSiteDirectory(child_dir, fullpath);
      continue;
    }

    AddPage(dest, NewPage(entry->d_name, fullpath));
  }

  closedir(dir);
}

static void BuildPage(const char *src_path, FILE *out_file) {
  FILE *file = fopen(src_path, "r");
  if (!file) {
    perror("Failed to open file");
    return;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Failed to seek to end");
    fclose(file);
    return;
  }

  long length = ftell(file);
  if (length < 0) {
    perror("Failed to tell file length");
    fclose(file);
    return;
  }

  if (fseek(file, 0, SEEK_SET) != 0) {
    perror("Failed to seek to start");
    fclose(file);
    return;
  }

  char *buffer = malloc(length + 1);
  if (!buffer) {
    perror("Failed to allocate memory");
    fclose(file);
    return;
  }

  size_t bytes_read = fread(buffer, 1, length, file);
  if (bytes_read != (size_t)length) {
    fprintf(stderr, "Failed to read file: expected %ld bytes, got %zu\n",
            length, bytes_read);
    free(buffer);
    fclose(file);
    return;
  }

  Lexer lexer = LexerNew(buffer, 0, length);

  Node *doc = NewNode(NODE_DOCUMENT);
  Parse(&lexer, doc);

  RenderHtml(doc, out_file);

  FreeNode(doc);
  free(buffer);
}

static void CopyFile(const char *src_path, const char *out_path) {
  FILE *in = fopen(src_path, "rb");
  if (!in) return;

  FILE *out = fopen(out_path, "wb");
  if (!out) {
    fclose(in);
    return;
  }

  char buffer[4096];
  size_t bytes;
  while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0) {
    if (fwrite(buffer, 1, bytes, out) != bytes) {
      fclose(in);
      fclose(out);
      return;
    }
  }

  fclose(in);
  fclose(out);
}

static void MkDir(const char *path) {
  int dir_result = mkdir(path, 0755);
  if (dir_result == 0 || dir_result == -1) {
  } else {
    perror("mkdir");
    return;
  }
}

static void InitializeSite() {
  MkDir("site");
  MkDir("site/assets");
  MkDir("site/assets/styles");
}

static void BuildSite(Directory *site_directory, const char *base_path) {
  if (site_directory->path[0] != '_') {
    MkDir(base_path);
  }

  for (size_t i = 0; i < site_directory->num_pages; i++) {
    Page *page      = site_directory->pages[i];
    const char *ext = strrchr(page->src_name, '.');
    if (strcmp(".css", ext) == 0) {
      char path[MAXFILEPATH];
      sprintf(path, "site/assets/styles/%s", page->src_name);
      CopyFile(page->full_path, path);
      continue;
    }

    char path[MAXFILEPATH];
    sprintf(path, "%s/%s", base_path, page->out_name);
    FILE *html_page        = fopen(path, "w");
    PageConfig page_config = (PageConfig){.page_title = page->src_name};
    TemplateStart(html_page, &page_config);
    BuildPage(page->full_path, html_page);
    TemplateEnd();
    fclose(html_page);
  }

  for (size_t i = 0; i < site_directory->num_dirs; i++) {
    Directory *directory = site_directory->dirs[i];
    char path[MAXFILEPATH];
    sprintf(path, "%s/%s", base_path, directory->path);
    BuildSite(directory, path);
  }
}

int main(void) {
  Directory *site_directory = NewDirectory("");
  BuildSiteDirectory(site_directory, DOCSDIR);
  InitializeSite();
  BuildSite(site_directory, BUILDDIR);

  Serve(BUILDDIR);

  return 0;
}