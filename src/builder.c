#include "builder.h"

static void AddPage(Directory *dir, Page *page) {
  assert(dir->num_dirs + 1 < MAXPAGESPERDIR);
  dir->dirs[dir->num_dirs++] = page;
  page->parent               = dir;
}

static void AddDirectory(Directory *dir, Directory *child) {
  assert(dir->num_dirs + 1 < MAXPAGESPERDIR);
  dir->dirs[dir->num_dirs++] = child;
  child->parent              = dir;
}

void BuildSiteDirectory(Directory *dest, const char *path, int level) {
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
      child_dir->level     = level;
      AddDirectory(dest, child_dir);
      BuildSiteDirectory(child_dir, fullpath, level + 1);
      continue;
    }

    Directory *page = NewPage(entry->d_name, fullpath);
    page->level     = level + 1;
    AddPage(dest, page);
  }

  closedir(dir);
}

static int GetHeadingLevel(Node *n) {
  if (strcmp("h1", n->type) == 0) return 1;
  if (strcmp("h2", n->type) == 0) return 2;
  if (strcmp("h3", n->type) == 0) return 3;
  if (strcmp("h4", n->type) == 0) return 4;
  if (strcmp("h5", n->type) == 0) return 5;
  if (strcmp("h6", n->type) == 0) return 6;
  return 0;
}

static void BuildToc(Node *doc, Page *page) {
  // TODO Improve check, this could mistake other elements starting with h.
  int heading_level = GetHeadingLevel(doc);
  if (heading_level) {
    // TODO Inefficient.
    size_t len = doc->end - doc->start;
    page->toc.items =
        realloc(page->toc.items, (page->toc.count + 1) * sizeof(TOCItem));
    page->toc.items[page->toc.count].link = malloc(len + 1);
    strncpy(page->toc.items[page->toc.count].link, &doc->input[doc->start],
            len);
    page->toc.items[page->toc.count].link[len]     = '\0';
    page->toc.items[page->toc.count].heading_level = heading_level;
    page->toc.count++;
  }

  if (doc->first_child) {
    BuildToc(doc->first_child, page);
  }

  if (doc->next_sibling) {
    BuildToc(doc->next_sibling, page);
  }
}

static void BuildPage(const char *src_path, FILE *out_file, Page *page) {
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

  Node *doc = NewNode("article");
  NodeAddAttribute(doc, "class", "gd-article");
  Parse(&lexer, doc);

  BuildToc(doc, page);

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

static void MkDirR(const char *path) {
  char tmp[1000];
  char buffer[1000] = {0};

  strcpy(tmp, path);
  char *tok = strtok(tmp, "/");

  while (tok != NULL) {
    if (buffer[0] != '\0') {
      strncat(buffer, "/", sizeof(buffer) - strlen(buffer) - 1);
    }
    strncat(buffer, tok, sizeof(buffer) - strlen(buffer) - 1);

    MkDir(buffer);

    tok = strtok(NULL, "/");

    // Don't create a directory if it's obviously a file
    if (tok) {
      char *ext = strrchr(tok, '.');
      if (ext) break;
    }
  }
}

static const char *BUILDDIR;

// TODO Make this better
void InitializeSite(const char *dir) {
  BUILDDIR = dir;
  char buff[100];
  sprintf(buff, "%s/assets/styles", BUILDDIR);
  MkDirR(buff);
}

void BuildSite(Directory *site_directory, Directory *current_directory,
               const char *base_path) {
  if (current_directory->path[0] != '_') {
    MkDir(base_path);
  }

  bool config_updated = false;

  for (size_t i = 0; i < current_directory->num_dirs; i++) {
    Page *page = current_directory->dirs[i];
    if (page->is_dir) continue;

    const char *ext = strrchr(page->src_name, '.');
    if (strcmp(".css", ext) == 0) {
      char p[MAXFILEPATH];
      strcpy(p, page->full_path);
      char *tok = strtok(p, "/");     // strip docs directory
      tok       = strtok(NULL, "/");  // strip _styles
      tok       = strtok(NULL, "");   // css file with directory
      char path[MAXFILEPATH];
      sprintf(path, "%s/assets/styles/%s", BUILDDIR, tok);
      MkDirR(path);

      struct stat source_stat, target_stat;
      bool source_exists = stat(page->full_path, &source_stat) == 0;
      bool target_exists = stat(path, &target_stat) == 0;
      if (source_exists && target_exists &&
          difftime(source_stat.st_mtime, target_stat.st_mtime) <= 0) {
        continue;
      }

      CopyFile(page->full_path, path);
      continue;
    }

    char path[MAXFILEPATH];
    sprintf(path, "%s/%s", base_path, page->out_name);
    strcpy(page->url, path);

    if (!config_updated) {
      struct stat source_stat, target_stat, config_stat;
      bool source_exists = stat(page->full_path, &source_stat) == 0;
      bool target_exists = stat(path, &target_stat) == 0;
      bool config_exists = stat("gendoc.config", &config_stat) == 0;
      if (source_exists && config_exists &&
          difftime(target_stat.st_mtime, config_stat.st_mtime) <= 0) {
        config_updated = true;
      }
      if (source_exists && target_exists &&
          difftime(source_stat.st_mtime, target_stat.st_mtime) <= 0) {
        if (!config_updated) {
          continue;
        }
      }
    }

    FILE *html_page = fopen(path, "w");
    LoadConfig();

    TemplateInit("templates/index.html", html_page);
    TemplateState state =
        TemplatePage(page, site_directory, current_directory, BuildPage);
    while (state.state != TEMPLATE_END) {
      state = TemplatePage(page, site_directory, current_directory, BuildPage);
    }
    TemplateDestroy();
    UnloadConfig();

    fclose(html_page);
  }

  for (size_t i = 0; i < current_directory->num_dirs; i++) {
    Directory *directory = current_directory->dirs[i];
    char path[MAXFILEPATH];
    sprintf(path, "%s/%s", base_path, directory->path);
    BuildSite(site_directory, directory, path);
  }
}