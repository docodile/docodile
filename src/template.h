#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

typedef struct {
  const char *site_name;
  const char *page_title;
  const char *description;
} PageConfig;

typedef struct {
  char label[100];
  char url[100];
} NavItem;

typedef struct {
  size_t items_count;
  NavItem items[100];
} Nav;

void TemplateStart(FILE *out_file, PageConfig *page_config, Nav *nav);
void TemplateEnd();

void FreeNav(Nav *nav);

#endif  // TEMPLATE_H