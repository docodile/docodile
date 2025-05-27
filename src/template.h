#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "directory.h"

void TemplateStart(FILE *out_file, Page *page, Directory *site_directory,
                   Directory *current_directory);
void TemplateEnd(Page *page);

#endif  // TEMPLATE_H