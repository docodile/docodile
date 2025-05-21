#include "template.h"

#include "assert.h"

static FILE *_out;
static PageConfig *_page_config;

#define print(fmt, ...) fprintf(_out, fmt "\n", ##__VA_ARGS__)

void TemplateStart(FILE *out_file, PageConfig *page_config) {
  _out         = out_file;
  _page_config = page_config;

  print("<!DOCTYPE html>");
  print("<html>");
  print("<head>");
  print("<meta charset=\"UTF-8\">");
  print("<meta name=\"viewport\" ");
  print("content=\"width=device-width,initial-scale=1.0\">");
  print("<title>%s</title>", _page_config->page_title);
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
  print("<body data-gd-color-scheme=\"%s\">", ReadConfig("theme"));
  print("<header class=\"gd-header\">");
  print("<h1>gendoc</h1>");
  print("<nav>");
  print("<ul>");
  print("<li><a href=\"/\">Home</a></li>");
  print("<li><a href=\"/getting-started/index.html\">Getting started</a></li>");
  print("<li><a href=\"/setup/index.html\">Setup</a></li>");
  print("<li><a href=\"\">About</a></li>");
  print("</ul>");
  print("</nav>");
  print("</header>");
  print("<main>");
  print("<aside class=\"gd-nav\">");
  print("</aside>");
}

void TemplateEnd() {
  assert(_out);
  assert(_page_config);
  print("<aside class=\"gd-toc\">");
  print("</aside>");
  print(
      "</main>\n"
      "<footer class=\"gd-footer\"></footer>\n"
      "</body>\n"
      "</html>");
}