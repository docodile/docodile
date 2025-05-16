#include "template.h"

#include "assert.h"

static FILE *out;

#define print(...) fprintf(out, __VA_ARGS__)

void TemplateStart(FILE *out_file) {
  out = out_file;

  // TODO build outer html template
  print(
      "<!DOCTYPE html>\n"
      "<html>\n"
      "<head>\n"
      "<meta charset=\"UTF-8\">"
      "<meta name=\"viewport\" content=\"width=device-width, "
      "initial-scale=1.0\">\n"
      // TODO Make the format of this configurable e.g. "SITENAME | PAGE".
      "<title>gendoc</title>\n"
      // TODO Make this configurable.
      "<meta name=\"description\" content=\"Put description here.\">\n"
      // TODO Make this configurable.
      "<meta name=\"author\" content=\"Your Name or Company\">\n"
      "<link rel=\"icon\" href=\"/favicon.ico\" type=\"image/x-icon\">\n"
      // TODO Open Graph
      // TODO Twitter Card
      // TODO Add override styles.
      "<script "
      "src=\"https://cdnjs.cloudflare.com/ajax/libs/prism/9000.0.1/"
      "prism.min.js\" "
      "integrity=\"sha512-UOoJElONeUNzQbbKQbjldDf9MwOHqxNz49NNJJ1d90yp+"
      "X9edsHyJoAs6O4K19CZGaIdjI5ohK+O2y5lBTW6uQ==\" "
      "crossorigin=\"anonymous\" referrerpolicy=\"no-referrer\"></script>\n"
      "<link rel=\"stylesheet\" "
      "href=\"https://cdnjs.cloudflare.com/ajax/libs/prism/9000.0.1/themes/"
      "prism.min.css\" "
      "integrity=\"sha512-/mZ1FHPkg6EKcxo0fKXF51ak6Cr2ocgDi5ytaTBjsQZIH/"
      "RNs6GF6+oId/vPe3eJB836T36nXwVh/WBl/cWT4w==\" "
      "crossorigin=\"anonymous\" referrerpolicy=\"no-referrer\" />\n"
      "<link rel=\"stylesheet\" href=\"assets/reset.css\">\n"
      "<link rel=\"stylesheet\" href=\"assets/styles.css\">\n"
      "</head>\n"
      "<body>\n"
      // TODO Header
      "<header>\n"
      // TODO Site name
      "<h1>gendoc</h1>\n"
      // TODO Generate nav
      "<nav>\n"
      "<ul>\n"
      "<li><a href=\"/\">Home</a></li>\n"
      "<li><a href=\"/getting-started/index.html\">Getting "
      "started</a></li>\n"
      "<li><a href=\"/setup/index.html\">Setup</a></li>\n"
      "<li><a href=\"\">About</a></li>\n"
      "</ul>\n"
      "</nav>\n"
      "</header>\n"
      "<main>\n");
}

void TemplateEnd() {
  assert(out);
  print(
      "</main>\n"
      "<footer></footer>\n"
      "</body>\n"
      "</html>\n");
}