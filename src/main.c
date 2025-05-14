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
  char out_name[MAXFILENAMELEN];
  char src_name[MAXFILENAMELEN];
  char full_path[MAXFILEPATH];
} Page;

static void ChangeFilePathFormat(const char *from, const char *to,
                                 const char *in, char *out) {
  const char *ext = strrchr(in, '.');
  if (ext && strcmp(ext, from) == 0) {
    size_t base_len = ext - in;
    strncpy(out, in, base_len);
    strcpy(out + base_len, to);
  } else {
    strcpy(out, in);
  }
}

static Page *NewPage(const char *name, const char *fullpath) {
  Page *page = malloc(sizeof(Page));
  ChangeFilePathFormat(".md", ".html", name, page->out_name);
  strcpy(page->src_name, name);
  strcpy(page->full_path, fullpath);
  return page;
}

typedef struct Directory {
  char path[MAXFILEPATH];
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

static void BuildPage(const char *src_path) {
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

  RenderHtml(doc);

  free(buffer);
  fclose(file);
}

static void BuildSite(Directory *site_directory, const char *base_path) {
  int dir_result = mkdir(base_path, 0755);
  // TODO -1 is File exists
  if (dir_result == 0 || dir_result == -1) {
  } else {
    perror("mkdir");
    return;
  }

  for (size_t i = 0; i < site_directory->num_pages; i++) {
    char path[MAXFILEPATH];
    sprintf(path, "%s/%s", base_path, site_directory->pages[i]->out_name);
    FILE *html_page = freopen(path, "w", stdout);
    BuildPage(site_directory->pages[i]->full_path);
  }

  for (size_t i = 0; i < site_directory->num_dirs; i++) {
    char path[MAXFILEPATH];
    sprintf(path, "%s/%s", base_path, site_directory->dirs[i]->path);
    BuildSite(site_directory->dirs[i], path);
  }
}

int main(void) {
  Directory *site_directory = NewDirectory("");
  BuildSiteDirectory(site_directory, DOCSDIR);
  BuildSite(site_directory, BUILDDIR);

  return 0;
}