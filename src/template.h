#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "directory.h"

typedef struct {
  char *input;
  size_t pos;
} Template;

typedef enum { TEMPLATE_START, TEMPLATE_YIELD, TEMPLATE_END } TemplateStateEnum;

typedef struct {
  TemplateStateEnum state;
  char *slot_name;
} TemplateState;

void TemplateInit(const char *template_file_path, FILE *out_file);
TemplateState TemplateBuild(Page *page);
void TemplateDestroy();

void TemplateNav(Directory *site_dir, Directory *current_dir);
void TemplateBackButton(Directory *site_dir, Directory *curr_dir);
void TemplateSideNav(Page *page, Directory *site_directory,
                     Directory *current_directory);
void TemplateToc(TOC toc);
void TemplateFooterNav(Page *page, Directory *site_directory,
                       Directory *current_directory);

#endif  // TEMPLATE_H