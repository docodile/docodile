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
          print("%s", value);
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
    Directory *dir = site_dir->dirs[i];
    if (dir->path[0] == '_') continue;
    char label[100];
    RemoveExtension(dir->path, label);
    KebabCaseToTitleCase(label, label);
    print("<li>");
    // HACK Update server to not need explicit /index.html
    print("<a href=\"/%s/index.html\">%s</a>", dir->path, label);
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
    print("<a class=\"gd-back\" href=\"%s\">%s</a>", parent->pages[0]->url_path,
          parent->title);
  }
}

void TemplateSideNav(Page *page, Directory *site_directory,
                     Directory *current_directory) {
  if (site_directory == NULL) return;
  if (current_directory == NULL) return;

  print("<ul>");
  for (size_t i = 0; i < current_directory->num_pages; i++) {
    print("<li><a href=\"%s\">%s</a></li>",
          current_directory->pages[i]->url_path,
          current_directory->pages[i]->title);
  }

  if (site_directory != current_directory) {
    for (size_t i = 0; i < current_directory->num_dirs; i++) {
      if (current_directory->dirs[i]->path[0] == '_') continue;
      print("<details>");
      print("<summary>%s</summary>", current_directory->dirs[i]->title);
      TemplateSideNav(page, site_directory, current_directory->dirs[i]);
      print("</details>");
    }
  }
  print("</ul>");
}

void TemplateToc(TOC toc) {
  print("<nav>");
  print("<ul>");
  for (size_t i = 0; i < toc.count; i++) {
    char link[100];
    TitleCaseToKebabCase(toc.items[i], link);
    print("<li><a href=\"#%s\">%s</a></li>", link, toc.items[i]);
  }
  print("</ul>");
  print("</nav>");
}

// <nav class="gd-footer-nav">
//   <a class="gd-nav-link prev">
//     <div>
//       <small>Prev</small>
//       <h2>Some article</h2>
//     </div>
//     <i class="bi bi-arrow-left-short"></i>
//   </a>
//   <a class="gd-nav-link next">
//     <div>
//       <small>Next</small>
//       <h2>Some article</h2>
//     </div>
//     <i class="bi bi-arrow-right-short"></i>
//   </a>
// </nav>

void TemplateFooterNav(Page *page, Directory *site_directory,
                       Directory *current_directory) {
  // int level = 0;
  // Directory *parent =
  //     FindParentDirectory(site_directory, current_directory, &level);

  size_t num_siblings =
      current_directory->num_dirs + current_directory->num_pages;
  size_t i = 0;
  for (; i < current_directory->num_pages; i++) {
    if (current_directory->pages[i] == page) break;
  }

  if (num_siblings < 2) return;

  print("<nav class=\"gd-footer-nav\">");
  if (i > 0) {
    print("<a class=\"gd-nav-link prev\" href=\"%s\">",
          current_directory->pages[i - 1]->url_path);
    print("<div>");
    print("<small>Prev</small>");
    print("<h2>%s</h2>", current_directory->pages[i - 1]->title);
    print("</div>");
    print("<i class=\"bi bi-arrow-left-short\"></i>");
    print("</a>");
  }
  if (i < current_directory->num_pages - 1) {
    print("<a class=\"gd-nav-link next\" href=\"%s\">",
          current_directory->pages[i + 1]->url_path);
    print("<div>");
    print("<small>Next</small>");
    print("<h2>%s</h2>", current_directory->pages[i + 1]->title);
    print("</div>");
    print("<i class=\"bi bi-arrow-right-short\"></i>");
    print("</a>");
  }
  print("</nav>");
}