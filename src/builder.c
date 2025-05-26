#include "builder.h"

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

  fclose(file);

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

static char *BUILDDIR;

void InitializeSite(const char *dir) {
  BUILDDIR = dir;
  MkDir(dir);
  char buff[100];
  sprintf(buff, "%s/assets", dir);
  MkDir(buff);
  sprintf(buff, "%s/assets/styles", dir);
  MkDir(buff);
}

Nav *BuildNav(Directory *dir) {
  Nav *nav         = malloc(sizeof(Nav));
  nav->items_count = 0;
  for (size_t i = 0; i < dir->num_dirs; i++) {
    if (dir->dirs[i]->path[0] != '_') {  // Ignore hidden directories.
      NavItem nav_item = {};
      strcpy(nav_item.url, dir->dirs[i]->path);
      char label[100];
      KebabCaseToTitleCase(dir->dirs[i]->path, label);
      strcpy(nav_item.label, label);
      nav->items[nav->items_count++] = nav_item;
    }
  }
  return nav;
}

void BuildSite(Directory *site_directory, const char *base_path, Nav *nav) {
  if (site_directory->path[0] != '_') {
    MkDir(base_path);
  }

  for (size_t i = 0; i < site_directory->num_pages; i++) {
    Page *page      = site_directory->pages[i];
    const char *ext = strrchr(page->src_name, '.');
    if (strcmp(".css", ext) == 0) {
      char path[MAXFILEPATH];
      sprintf(path, "%s/assets/styles/%s", BUILDDIR, page->src_name);
      CopyFile(page->full_path, path);
      continue;
    }

    char path[MAXFILEPATH];
    sprintf(path, "%s/%s", base_path, page->out_name);
    strcpy(page->url, path);
    FILE *html_page        = fopen(path, "w");
    LoadConfig();
    TemplateStart(html_page, page, nav, site_directory);
    BuildPage(page->full_path, html_page);
    TemplateEnd();
    fclose(html_page);
  }

  for (size_t i = 0; i < site_directory->num_dirs; i++) {
    Directory *directory = site_directory->dirs[i];
    char path[MAXFILEPATH];
    sprintf(path, "%s/%s", base_path, directory->path);
    BuildSite(directory, path, nav);
  }
}