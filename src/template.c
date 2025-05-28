#include "template.h"

#include "assert.h"

static FILE *_out;
static Page *_page;

#define print(fmt, ...) fprintf(_out, fmt "\n", ##__VA_ARGS__)

static void BuildNav(Directory *site_dir, Directory *current_dir) {
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

Directory *FindParentDirectory(Directory *root, Directory *target, int *level) {
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

static void BuildBackButton(Directory *site_dir, Directory *curr_dir) {
  if (site_dir == NULL) return;
  if (curr_dir == NULL) return;
  if (site_dir == curr_dir) return;

  // Find parent directory.
  int level         = 0;
  Directory *parent = FindParentDirectory(site_dir, curr_dir, &level);
  if (parent && level > 0) {
    char article_link[MAXURL];
    strcpy(article_link, parent->pages[0]->url_path);
    strtok(article_link, "/");
    char *path = strtok(NULL, "");
    print("<a class=\"gd-back\" href=\"/%s\">%s</a>", path, parent->title);
  }
}

static void BuildSideNav(Page *page, Directory *site_directory,
                         Directory *current_directory) {
  if (site_directory == NULL) return;
  if (current_directory == NULL) return;

  print("<ul>");
  for (size_t i = 0; i < current_directory->num_pages; i++) {
    char article_link[MAXURL];
    strcpy(article_link, current_directory->pages[i]->url_path);
    strtok(article_link, "/");
    char *path  = strtok(NULL, "");
    char *title = current_directory->pages[i]->title;
    if (strcmp("index.md", current_directory->pages[i]->src_name) == 0) {
      title = current_directory->title;
    }
    print("<li><a href=\"/%s\">%s</a></li>", path, title);
  }

  if (site_directory != current_directory) {
    for (size_t i = 0; i < current_directory->num_dirs; i++) {
      if (current_directory->dirs[i]->path[0] == '_') continue;
      print("<details>");
      print("<summary>%s</summary>", current_directory->dirs[i]->title);
      BuildSideNav(page, site_directory, current_directory->dirs[i]);
      print("</details>");
    }
  }
  print("</ul>");
}

void TemplateStart(FILE *out_file, Page *page, Directory *site_directory,
                   Directory *current_directory) {
  _out  = out_file;
  _page = page;

  print("<!DOCTYPE html>");
  print("<html>");
  print("<head>");
  print("<meta charset=\"UTF-8\">");
  print("<meta name=\"viewport\" ");
  print("content=\"width=device-width,initial-scale=1.0\">");
  print("<title>%s</title>", _page->out_name);
  print("<meta name=\"description\" content=\"Put description here.\">");
  print("<meta name=\"author\" content=\"Your Name or Company\">");
  print(
      "<link rel=\"icon\" href=\"/assets/favicon.ico\" type=\"image/x-icon\">");
  // TODO Open Graph
  // TODO Twitter Card
  print(
      "<link rel=\"stylesheet\" type=\"text/css\" "
      "href=\"/assets/styles/reset.css\">");
  print(
      "<link rel=\"stylesheet\" type=\"text/css\" "
      "href=\"/assets/styles/vars.css\">");
  print(
      "<link rel=\"stylesheet\" type=\"text/css\" "
      "href=\"/assets/styles/main.css\">");
  print(
      "<link rel=\"stylesheet\" "
      "href=\"https://cdn.jsdelivr.net/npm/bootstrap-icons@1.13.1/font/"
      "bootstrap-icons.min.css\">");
  print(
      "<link rel=\"stylesheet\" type=\"text/css\" "
      "href=\"/assets/styles/default-prism-theme.css\">");
  // Google fonts
  print("<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">");
  print(
      "<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" "
      "crossorigin>");
  print(
      "<link href=\"https://fonts.googleapis.com/css2?family=%s&display=swap\" "
      "rel=\"stylesheet\">",
      ReadConfig("font-family"));
  // Google fonts
  print("</head>");
  print(
      "<body data-gd-color-scheme=\"%s\" data-gd-accent-color=\"%s\" "
      "style=\"font-family: '%s'\">",
      ReadConfig("theme.color-scheme"), ReadConfig("theme.accent-color"),
      ReadConfig("font-family"));
  print("<header class=\"gd-header\">");
  print("<a href=\"/\" class=\"gd-title\"><h1>%s</h1></a>",
        ReadConfig("site-name"));
  BuildNav(site_directory, current_directory);
  print("</header>");
  print("<main>");
  print("<aside class=\"gd-nav\">");
  print("<nav>");
  BuildBackButton(site_directory, current_directory);
  BuildSideNav(_page, site_directory, current_directory);
  print("</nav>");
  print("</aside>");
}

static void BuildToc(TOC toc) {
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

void TemplateEnd(Page *page) {
  assert(_out);
  assert(_page);
  print("<aside class=\"gd-toc\">");
  BuildToc(page->toc);
  print("</aside>");
  print("</main>");
  print(
      "<footer class=\"gd-footer\">Built with <span "
      "class=\"highlight\">gendoc</span></footer>\n");
  print(
      "<script "
      "src=\"https://cdnjs.cloudflare.com/ajax/libs/prism/1.30.0/components/"
      "prism-core.min.js\" "
      "integrity=\"sha512-Uw06iFFf9hwoN77+kPl/"
      "1DZL66tKsvZg6EWm7n6QxInyptVuycfrO52hATXDRozk7KWeXnrSueiglILct8IkkA==\" "
      "crossorigin=\"anonymous\" referrerpolicy=\"no-referrer\"></script>");
  print(
      "<script "
      "src=\"https://cdnjs.cloudflare.com/ajax/libs/prism/1.30.0/plugins/"
      "autoloader/prism-autoloader.min.js\" "
      "integrity=\"sha512-SkmBfuA2hqjzEVpmnMt/"
      "LINrjop3GKWqsuLSSB3e7iBmYK7JuWw4ldmmxwD9mdm2IRTTi0OxSAfEGvgEi0i2Kw==\" "
      "crossorigin=\"anonymous\" referrerpolicy=\"no-referrer\"></script>");
  print("</body>");
  print("</html>");
}