#include "template.h"

#include "assert.h"

static FILE *_out;
static Page *_page;

#define print(fmt, ...) fprintf(_out, fmt "\n", ##__VA_ARGS__)

void FreeNav(Nav *nav) { free(nav); }

static void BuildNav(Nav *nav) {
  print("<nav>");
  print("<ul>");
  for (size_t i = 0; i < nav->items_count; i++) {
    print("<li>");
    print("<a href=\"/%s/index.html\">%s</a>", nav->items[i].url,
          nav->items[i]
              .label);  // HACK Update server to not need explicit /index.html
    print("</li>");
  }
  print("</ul>");
  print("</nav>");
}

static void BuildSideNav(Page *page, Directory *dir) {
  print("<ul>");
  if (dir != NULL) {
    for (size_t i = 0; i < dir->num_pages; i++) {
      char article_link[MAXURL];
      strcpy(article_link, dir->pages[i]->url_path);
      strtok(article_link, "/");
      char *path  = strtok(NULL, "");
      char *title = dir->pages[i]->title;
      if (strcmp("index.md", dir->pages[i]->src_name) == 0) {
        title = dir->title;
      }
      print("<li><a href=\"/%s\">%s</a></li>", path, title);
    }

    for (size_t i = 0; i < dir->num_dirs; i++) {
      print("<details>");
      print("<summary>%s</summary>", dir->dirs[i]->title);
      BuildSideNav(page, dir->dirs[i]);
      print("</details>");
    }
  }
  print("</ul>");
}

void TemplateStart(FILE *out_file, Page *page, Nav *nav,
                   Directory *site_directory) {
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
  print("</head>");
  print("<body data-gd-color-scheme=\"%s\" data-gd-accent-color=\"%s\">",
        ReadConfig("theme.color-scheme"), ReadConfig("theme.accent-color"));
  print("<header class=\"gd-header\">");
  print("<h1>%s</h1>", ReadConfig("site-name"));
  BuildNav(nav);
  print("</header>");
  print("<main>");
  print("<aside class=\"gd-nav\">");
  print("<nav>");
  BuildSideNav(_page, site_directory);
  print("</nav>");
  print("</aside>");
}

void TemplateEnd() {
  assert(_out);
  assert(_page);
  print("<aside class=\"gd-toc\">");
  print("</aside>");
  print(
      "</main>\n"
      "<footer class=\"gd-footer\"></footer>\n"
      "</body>\n"
      "</html>");
}