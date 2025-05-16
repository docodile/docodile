#include "template.h"

#include "assert.h"

static FILE *out;
static PageConfig *config;

#define print(fmt, ...) fprintf(out, fmt "\n", ##__VA_ARGS__)

void TemplateStart(FILE *out_file, PageConfig *page_config) {
  out    = out_file;
  config = page_config;

  print("<!DOCTYPE html>");
  print("<html>");
  print("<head>");
  print("<meta charset=\"UTF-8\">");
  print("<meta name=\"viewport\" ");
  print("content=\"width=device-width,initial-scale=1.0\">");
  print("<title>%s</title>", config->page_title);
  print("<meta name=\"description\" content=\"Put description here.\">");
  print("<meta name=\"author\" content=\"Your Name or Company\">");
  print(
      "<link rel=\"icon\" href=\"/assets/favicon.ico\" type=\"image/x-icon\">");
  // TODO Open Graph
  // TODO Twitter Card
  print("<link rel=\"stylesheet\" href=\"/assets/reset.css\">");
  print("<link rel=\"stylesheet\" href=\"/assets/styles.css\">");
  print("</head>");
  print("<body>");
  print("<header>");
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
}

void TemplateEnd() {
  assert(out);
  assert(config);
  print(
      "</main>\n"
      "<footer></footer>\n"
      "</body>\n"
      "</html>");
}