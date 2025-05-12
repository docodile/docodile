#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "html.h"
#include "lex.h"
#include "logger.h"
#include "parser.h"

#define BUILDDIR       "site"
#define DOCSDIR        "docs"
#define DOCSMAXDEPTH   1000
#define MAXDIRS        1000
#define MAXPAGESPERDIR 1000
#define MAXFILENAMELEN 1000
#define MAXFILEPATH    1000

typedef struct {
  char src_name[MAXFILENAMELEN];
  char full_path[MAXFILEPATH];
} Page;

static Page *NewPage(const char *name, const char *fullpath) {
  Page *page = malloc(sizeof(Page));
  strcpy(page->src_name, name);
  strcpy(page->full_path, fullpath);
  return page;
}

typedef struct Directory {
  const char *path;
  size_t num_pages;
  size_t num_dirs;
  Page *pages[MAXPAGESPERDIR];
  struct Directory *dirs[MAXDIRS];
} Directory;

// TODO FreeDirectory function
static Directory *NewDirectory(const char *path) {
  Directory *dir = malloc(sizeof(Directory));
  dir->path      = path;
  dir->num_pages = 0;
  dir->num_dirs  = 0;
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
    // Skip "." and ".."
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    // Construct full path
    char fullpath[DOCSMAXDEPTH];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

    struct stat statbuf;
    if (stat(fullpath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
      Directory *child_dir = NewDirectory(entry->d_name);
      AddDirectory(dest, child_dir);
      BuildSiteDirectory(child_dir, fullpath);  // Recursive call
      continue;
    }

    AddPage(dest, NewPage(entry->d_name, fullpath));

    fprintf(stderr, "%s\n", fullpath);
  }

  closedir(dir);
}

int main(void) {
  Directory *site_directory = NewDirectory("");
  BuildSiteDirectory(site_directory, DOCSDIR);

  FILE *file = fopen("docs/index.md", "r");
  if (!file) {
    perror("Failed to open file");
    return 1;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Failed to seek to end");
    fclose(file);
    return 1;
  }

  long length = ftell(file);
  if (length < 0) {
    perror("Failed to tell file length");
    fclose(file);
    return 1;
  }

  if (fseek(file, 0, SEEK_SET) != 0) {
    perror("Failed to seek to start");
    fclose(file);
    return 1;
  }

  char *buffer = malloc(length + 1);
  if (!buffer) {
    perror("Failed to allocate memory");
    fclose(file);
    return 1;
  }

  size_t bytes_read = fread(buffer, 1, length, file);
  if (bytes_read != (size_t)length) {
    fprintf(stderr, "Failed to read file: expected %ld bytes, got %zu\n",
            length, bytes_read);
    free(buffer);
    fclose(file);
    return 1;
  }

  Lexer lexer = LexerNew(buffer, 0, length);

  Node *doc = NewNode(NODE_DOCUMENT);
  Parse(&lexer, doc);

  RenderHtml(doc);

  free(buffer);
  fclose(file);

  return 0;
}