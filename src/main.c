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

  free(buffer);
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
    Page *page = site_directory->pages[i];
    char path[MAXFILEPATH];
    sprintf(path, "%s/%s", base_path, page->out_name);
    FILE *html_page = fopen(path, "w");

#define print(...) fprintf(html_page, __VA_ARGS__)

    // TODO build outer html template
    print("<!DOCTYPE html>\n");
    print("<html>\n");
    print("<head>\n");
    print("<meta charset=\"UTF-8\">");
    print(
        "<meta name=\"viewport\" content=\"width=device-width, "
        "initial-scale=1.0\">\n");
    // TODO Make the format of this configurable e.g. "SITENAME | PAGE".
    print("<title>gendoc</title>\n");
    // TODO Make this configurable.
    print("<meta name=\"description\" content=\"Put description here.\">\n");
    // TODO Make this configurable.
    print("<meta name=\"author\" content=\"Your Name or Company\">\n");
    print("<link rel=\"icon\" href=\"/favicon.ico\" type=\"image/x-icon\">\n");
    // TODO Open Graph
    // TODO Twitter Card
    // TODO Add override styles.
    print(
        "<script "
        "src=\"https://cdnjs.cloudflare.com/ajax/libs/prism/9000.0.1/"
        "prism.min.js\" "
        "integrity=\"sha512-UOoJElONeUNzQbbKQbjldDf9MwOHqxNz49NNJJ1d90yp+"
        "X9edsHyJoAs6O4K19CZGaIdjI5ohK+O2y5lBTW6uQ==\" "
        "crossorigin=\"anonymous\" referrerpolicy=\"no-referrer\"></script>\n");
    print(
        "<link rel=\"stylesheet\" "
        "href=\"https://cdnjs.cloudflare.com/ajax/libs/prism/9000.0.1/themes/"
        "prism.min.css\" "
        "integrity=\"sha512-/mZ1FHPkg6EKcxo0fKXF51ak6Cr2ocgDi5ytaTBjsQZIH/"
        "RNs6GF6+oId/vPe3eJB836T36nXwVh/WBl/cWT4w==\" "
        "crossorigin=\"anonymous\" referrerpolicy=\"no-referrer\" />\n");
    print("<link rel=\"stylesheet\" href=\"assets/reset.css\">\n");
    print("<link rel=\"stylesheet\" href=\"assets/styles.css\">\n");
    print("</head>\n");
    print("<body>\n");
    // TODO Header
    print("<header>\n");
    // TODO Site name
    print("<h1>gendoc</h1>\n");

    // TODO Generate nav
    print("<nav>\n");
    print("<ul>\n");
    print("<li><a href=\"/\">Home</a></li>\n");
    print(
        "<li><a href=\"/getting-started/index.html\">Getting "
        "started</a></li>\n");
    print("<li><a href=\"/setup/index.html\">Setup</a></li>\n");
    print("<li><a href=\"\">About</a></li>\n");
    print("</ul>\n");
    print("</nav>\n");

    print("</header>\n");

    print("<main>\n");

    BuildPage(page->full_path, html_page);

    print("</main>\n");

    print("<footer></footer>\n");
    print("</body>\n");
    print("</html>\n");
    // TODO build outer html template

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
  BuildSite(site_directory, BUILDDIR);

  Serve(BUILDDIR);

  return 0;
}