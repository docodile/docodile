#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <stdio.h>
#include "config.h"

typedef struct {
  const char *site_name;
  const char *page_title;
  const char *description;
} PageConfig;

void TemplateStart(FILE *out_file, PageConfig *page_config);
void TemplateEnd();

#endif  // TEMPLATE_H