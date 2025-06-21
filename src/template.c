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
        char *slot_name = (char *)malloc(i + 1);
        strcpy(slot_name, buffer);
        return (TemplateState){.state = TEMPLATE_YIELD, .slot_name = slot_name};
      }
    }

    Put(c);
  }

  return (TemplateState){.state = TEMPLATE_END};
}

TemplateState TemplatePage(Page *page, Directory *site_directory,
                           Directory *current_directory,
                           BuildPageFunc build_page_func) {
  TemplateState state;
  while ((state = TemplateBuild(page)).state != TEMPLATE_END) {
    if (state.state == TEMPLATE_YIELD) {
      if (strcmp("article", state.slot_name) == 0)
        build_page_func(page->full_path, _out, page);
      if (strcmp("toc", state.slot_name) == 0) TemplateToc(page->toc);
      if (strcmp("nav", state.slot_name) == 0)
        TemplateNav(site_directory, current_directory);
      if (strcmp("nav_back_button", state.slot_name) == 0)
        TemplateBackButton(site_directory, current_directory);
      if (strcmp("side_nav", state.slot_name) == 0)
        TemplateSideNav(page, site_directory, current_directory, false);
      if (strcmp("footer_nav", state.slot_name) == 0)
        TemplateFooterNav(page, site_directory, current_directory);
      if (strcmp("breadcrumbs", state.slot_name) == 0) {
        char *breadcrumbs_config = ReadConfig("navigation.breadcrumbs");
        if (breadcrumbs_config && strcmp("true", breadcrumbs_config) == 0) {
          TemplateBreadcrumbs(page, site_directory, current_directory);
        }
      }
      if (strcmp("main", state.slot_name) == 0) {
        if (page->is_index && page->level == 1) {
          TemplatePartial("partials/home.html", page, site_directory,
                          current_directory, build_page_func);
        } else {
          TemplatePartial("partials/main.html", page, site_directory,
                          current_directory, build_page_func);
        }
      }
      if (HasExtension(state.slot_name, ".html"))
        TemplatePartial(state.slot_name, page, site_directory,
                        current_directory, build_page_func);

      if (state.slot_name) {
        free(state.slot_name);
      }
    }
  }
  return state;
}

static Directory *FindIndexPage(Directory *dir) {
  for (size_t j = 0; j < dir->num_dirs; j++) {
    if (dir->dirs[j]->is_index) {
      return dir->dirs[j];
    }
  }

  return NULL;
}

static bool IsActive(Directory *dir, Directory *current_dir) {
  if (dir == current_dir) return true;

  Directory *parent = current_dir->parent;
  while (parent != NULL) {
    if (parent == dir) return true;
    parent = parent->parent;
  }

  return false;
}

void TemplateNav(Directory *site_dir, Directory *current_dir) {
  print("<nav>");
  print("<menu>");
  for (size_t i = 0; i < site_dir->num_dirs; i++) {
    Directory *dir = site_dir->dirs[i];
    if (!dir->is_dir) continue;
    if (dir->path[0] == '_') continue;
    if (IsActive(dir, current_dir))
      print("<li class=\"active\">");
    else
      print("<li>");
    print("<a href=\"/%s/index.html\">%s</a>", dir->path, dir->title);
    print("</li>");
  }
  print("</menu>");
  print("</nav>");
}

void TemplateBackButton(Directory *site_dir, Directory *curr_dir) {
  if (site_dir == NULL) return;
  if (curr_dir == NULL) return;
  if (site_dir == curr_dir) return;

  Directory *parent = curr_dir->parent;
  if (parent && curr_dir->level > 0) {
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
                     Directory *current_directory, bool hide_index) {
  if (site_directory == NULL) return;
  if (current_directory == NULL) return;

  print("<ul>");
  for (size_t i = 0; i < current_directory->num_dirs; i++) {
    Directory *dir = current_directory->dirs[i];
    if (dir->is_dir) {
      if (current_directory == site_directory) continue;
      if (dir->path[0] == '_') continue;
      print("<details>");

      Directory *index_page = FindIndexPage(dir);

      if (index_page != NULL) {
        print("<summary><a href=\"%s\">%s</a></summary>", index_page->url_path,
              dir->title);
      } else {
        print("<summary>%s</summary>", dir->title);
      }
      TemplateSideNav(page, site_directory, dir, true);
      print("</details>");
    } else {
      if (strcmp("_nav.md", dir->src_name) == 0) continue;
      if (hide_index && dir->is_index) continue;
      char classes[100] = "";
      if (page == dir) sprintf(classes, " class=\"active\"");
      print("<li><a href=\"%s\"%s>%s</a></li>", dir->url_path, classes,
            dir->title);
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
    char *label = strtok(item.link, "{");
    print("<li><a class=\"toc-%d\" href=\"#%s\">%s</a></li>",
          item.heading_level, link, label);
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

void TemplatePartial(const char *partial_name, Page *page,
                     Directory *site_directory, Directory *current_directory,
                     BuildPageFunc build_page_func) {
  size_t saved_pos  = _template.pos;
  char *saved_input = _template.input;
  size_t len;
  char file_path[100];
  sprintf(file_path, "templates/%s", partial_name);
  char *template_source = ReadFileToString(file_path, &len);
  _template.pos         = 0;
  _template.input       = template_source;
  TemplatePage(page, site_directory, current_directory, build_page_func);
  _template.pos   = saved_pos;
  _template.input = saved_input;
}

void TemplateBreadcrumbs(Directory *page, Directory *site_directory,
                         Directory *current_directory) {
  print("<nav class=\"gd-breadcrumbs\">");
  print("<menu>");
  int level         = 0;
  Directory *parent = page->parent;
  bool skip         = page->level < 3 && page->is_index;
  if (!skip) {
    print("<li><span>%s</span></li>", page->title);
    while (parent != NULL) {
      Directory *index_page = FindIndexPage(parent);
      if (index_page != page) {
        if (parent->title[0] == '\0') {
        } else {
          print("<li><a href=\"%s\">%s</a></li>", index_page->url_path,
                parent->title);
        }
      }
      parent = parent->parent;
    }
  }
  print("</menu>");
  print("</nav>");
}