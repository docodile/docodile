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

  char title[1000];

  if (strcmp("index.md", second) == 0)
    strcpy(title, first);
  else
    strcpy(title, second);

  char *site_name = ReadConfig("site-name");

  if (strcmp("docs", title) == 0) {
    if (site_name) {
      strcpy(title, site_name);
    }
  }

  free(site_name);

  RemoveExtension(title, out);
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

static char *CreateCleanPath(const char *original, bool is_index) {
  size_t len = strlen(original);
  char *path = malloc(len + 1);
  memcpy(path, original, len);
  path[len] = '\0';
  if (is_index) {
    path[len - sizeof("/index.html")] = '\0';
  } else {
    path[len - sizeof(".html")] = '\0';
  }
  return path;
}

Page *NewPage(const char *name, const char *fullpath) {
  Page *page     = malloc(sizeof(Page));
  page->is_index = strcmp("index.md", name) == 0;
  ChangeFilePathExtension(".md", ".html", name, page->out_name);
  GenerateUrl(fullpath, page->url_path);
  page->clean_path = CreateCleanPath(page->url_path, page->is_index);
  GenerateTitle(fullpath, page->title);
  strcpy(page->src_name, name);
  strcpy(page->full_path, fullpath);
  strcpy(page->path, fullpath);
  page->hidden    = name[0] == '_';
  page->nav_index = __INT_MAX__;
  page->is_dir    = false;
  page->num_dirs  = 0;
  page->toc       = (TOC){.count = 0, .items = malloc(sizeof(TOCItem))};
  page->parent    = NULL;
  page->level     = 0;
  return page;
}

Directory *NewDirectory(const char *path) {
  Directory *dir = malloc(sizeof(Directory));
  KebabCaseToTitleCase(path, dir->title);
  dir->is_index = false;
  strcpy(dir->path, path);
  dir->nav_index    = __INT_MAX__;
  dir->src_name[0]  = '\0';
  dir->full_path[0] = '\0';
  dir->num_dirs     = 0;
  dir->hidden       = false;
  dir->is_dir       = true;
  dir->toc          = (TOC){.count = 0, .items = malloc(sizeof(TOCItem))};
  dir->parent       = NULL;
  dir->level        = 0;
  return dir;
}

static int CompareDirs(const void *a, const void *b) {
  Directory *dir_a = *(Directory **)a;
  Directory *dir_b = *(Directory **)b;

  if (dir_a->nav_index < dir_b->nav_index) return -1;
  if (dir_a->nav_index > dir_b->nav_index) return 1;
  if (dir_a->is_index && !dir_b->is_index) return -1;
  if (!dir_a->is_index && dir_b->is_index) return 1;
  return strcasecmp(dir_a->title, dir_b->title);
}

void SortDirectory(Directory *dir) {
  char *input = NULL;
  Node *nav   = NULL;
  for (size_t i = 0; i < dir->num_dirs; i++) {
    if (strcmp("_nav.md", dir->dirs[i]->src_name) == 0) {
      nav = NewNode("nav");
      size_t len;
      input       = ReadFileToString(dir->dirs[i]->full_path, &len);
      Lexer lexer = LexerNew(input, 0, len);
      Parse(&lexer, nav);
      break;
    }
  }

  if (nav) {
    size_t index = 0;
    Node *li     = nav->first_child;
    if (li != NULL) li = li->first_child;
    while (li != NULL && strcmp("li", li->type) == 0) {
      char buffer[1000];
      sprintf(buffer, "%.*s", li->end - li->start, &li->input[li->start]);
      for (size_t i = 0; i < dir->num_dirs; i++) {
        Directory *curr = dir->dirs[i];
        if ((curr->is_dir && strcmp(buffer, curr->path) == 0) ||
            (!curr->is_dir && strcmp(buffer, curr->src_name) == 0)) {
          dir->dirs[i]->nav_index = index++;
          break;
        }
      }
      li = li->next_sibling;
    }

    FreeNode(nav);
  }

  if (input) free(input);

  qsort(dir->dirs, dir->num_dirs, sizeof(Directory *), CompareDirs);

  for (size_t i = 0; i < dir->num_dirs; i++) {
    SortDirectory(dir->dirs[i]);
  }
}

void FreeDirectory(Directory *dir) {
  for (size_t i = 0; i < dir->num_dirs; i++) {
    FreeDirectory(dir->dirs[i]);
  }

  for (size_t i = 0; i < dir->toc.count; i++) free(dir->toc.items[i].link);
  free(dir->toc.items);
  free(dir->clean_path);
  free(dir);
}