#include "template.h"

static FILE *_out;
static Template _template;

#define print(fmt, ...) fprintf(_out, fmt, ##__VA_ARGS__)

static char *LoadTemplateFile(const char *filename) {
  size_t len;
  return ReadFileToString(filename, &len);
}

void TemplateInit(const char *template_file_path, FILE *out_file) {
  _template =
      (Template){.pos = 0, .input = LoadTemplateFile(template_file_path)};
  _out = out_file;
}

void TemplateDestroy() {
  if (_template.input) {
    free(_template.input);
  }
}

static char Peek() { return _template.input[_template.pos]; }
static char Advance() { return _template.input[_template.pos++]; }
static void Put(char c) { fputc(c, _out); }

TemplateState TemplateBuild(Page *page) {
  char c;
  while ((c = Advance()) != '\0') {
    if (c == '{') {
      // Simple template variable.
      if (Peek() == '{') {
        Advance();

        char buffer[100];
        size_t i = 0;
        while ((c = Advance()) != '\0' && c != '}') {
          if (c != ' ') buffer[i++] = c;
        }
        buffer[i] = '\0';
        Advance();  // Remaining closing brace

        char *prefix = strtok(buffer, ".");
        char *key    = strtok(NULL, "");
        if (strcmp("config", prefix) == 0) {
          char *value = ReadConfig(key);
          if (value) {
            print("%s", value);
          }
          free(value);
          continue;
        }

        if (strcmp("page", prefix) == 0) {
          if (strcmp("title", key) == 0) {
            print("%s", page->title);
          }

          continue;
        }

        continue;
      }

      // Slot. Consume token and yield.
      if (Peek() == '%') {
        Advance();
        char buffer[100];
        size_t i = 0;
        while ((c = Advance()) != '\0' && c != '%') {
          if (c != ' ') buffer[i++] = c;
        }
        buffer[i] = '\0';
        Advance();
        char *slot_name = (char *)malloc(i);
        strcpy(slot_name, buffer);
        return (TemplateState){.state = TEMPLATE_YIELD, .slot_name = slot_name};
      }
    }

    Put(c);
  }

  return (TemplateState){.state = TEMPLATE_END};
}

void TemplateNav(Directory *site_dir, Directory *current_dir) {
  print("<nav>");
  print("<ul>");
  for (size_t i = 0; i < site_dir->num_dirs; i++) {
    if (!site_dir->dirs[i]->is_dir) continue;
    Directory *dir = site_dir->dirs[i];
    if (dir->path[0] == '_') continue;
    print("<li>");
    // HACK Update server to not need explicit /index.html
    print("<a href=\"/%s/index.html\">%s</a>", dir->path, dir->title);
    print("</li>");
  }
  print("</ul>");
  print("</nav>");
}

static Directory *FindParentDirectory(Directory *root, Directory *target,
                                      int *level) {
  for (size_t i = 0; i < root->num_dirs; i++) {
    if (root->dirs[i] == target) return root;

    Directory *maybe = FindParentDirectory(root->dirs[i], target, level);
    if (maybe) {
      (*level)++;
      return maybe;
    }
  }

  return NULL;
}

void TemplateBackButton(Directory *site_dir, Directory *curr_dir) {
  if (site_dir == NULL) return;
  if (curr_dir == NULL) return;
  if (site_dir == curr_dir) return;

  // Find parent directory.
  int level         = 0;
  Directory *parent = FindParentDirectory(site_dir, curr_dir, &level);
  if (parent && level > 0) {
    Directory *first_index = NULL;
    for (size_t i = 0; i < parent->num_dirs; i++) {
      if (!parent->dirs[i]->hidden && parent->dirs[i]->is_index) {
        first_index = parent->dirs[i];
        break;
      }
    }
    if (first_index) {
      print("<a class=\"gd-back\" href=\"%s\">%s</a>", first_index->url_path,
            parent->title);
    }
  }
}

void TemplateSideNav(Page *page, Directory *site_directory,
                     Directory *current_directory) {
  if (site_directory == NULL) return;
  if (current_directory == NULL) return;

  print("<ul>");
  for (size_t i = 0; i < current_directory->num_dirs; i++) {
    if (current_directory->dirs[i]->is_dir) {
      if (current_directory == site_directory) continue;
      if (current_directory->dirs[i]->path[0] == '_') continue;
      print("<details>");
      print("<summary>%s</summary>", current_directory->dirs[i]->title);
      TemplateSideNav(page, site_directory, current_directory->dirs[i]);
      print("</details>");
    } else {
      if (strcmp("_nav.md", current_directory->dirs[i]->src_name) == 0) continue;
      char classes[100] = "";
      if (page == current_directory->dirs[i]) {
        sprintf(classes, " class=\"active\"");
      }
      print("<li><a href=\"%s\"%s>%s</a></li>",
            current_directory->dirs[i]->url_path, classes,
            current_directory->dirs[i]->title);
    }
  }
  print("</ul>");
}

void TemplateToc(TOC toc) {
  print("<nav>");
  print("<ul>");
  for (size_t i = 0; i < toc.count; i++) {
    TOCItem item = toc.items[i];
    char link[100];
    TitleCaseToKebabCase(item.link, link);
    print("<li><a class=\"toc-%d\" href=\"#%s\">%s</a></li>",
          item.heading_level, link, item.link);
  }
  print("</ul>");
  print("</nav>");
}

void TemplateFooterNav(Page *page, Directory *site_directory,
                       Directory *current_directory) {
  Directory *prev = NULL, *next = NULL;

  for (size_t i = 0; i < current_directory->num_dirs; i++) {
    Directory *this = current_directory->dirs[i];
    if (this == page) break;
    if (!this->hidden && !this->is_dir) prev = this;
  }

  for (size_t i = current_directory->num_dirs - 1; i >= 0; i--) {
    Directory *this = current_directory->dirs[i];
    if (this == page) break;
    if (!this->hidden && !this->is_dir) next = this;
  }

  print("<nav class=\"gd-footer-nav\">");
  if (prev) {
    print("<a class=\"gd-nav-link prev\" href=\"%s\">", prev->url_path);
    print("<div>");
    print("<small>Prev</small>");
    print("<h2>%s</h2>", prev->title);
    print("</div>");
    print("<i class=\"bi bi-arrow-left-short\"></i>");
    print("</a>");
  }
  if (next) {
    print("<a class=\"gd-nav-link next\" href=\"%s\">", next->url_path);
    print("<div>");
    print("<small>Next</small>");
    print("<h2>%s</h2>", next->title);
    print("</div>");
    print("<i class=\"bi bi-arrow-right-short\"></i>");
    print("</a>");
  }
  print("</nav>");
}