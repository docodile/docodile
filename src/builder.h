#ifndef BUILDER_H
#define BUILDER_H

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "directory.h"
#include "html.h"
#include "parser.h"
#include "template.h"
#include "utils.h"

#define DOCSDIR      "docs"
#define DOCSMAXDEPTH 1000

void BuildSiteDirectory(Directory *dest, const char *path, int level);
void InitializeSite(const char *dir);
void BuildSite(Directory *site_directory, Directory *current_directory,
               const char *base_path);
void Build404Page(Directory *dest, const char *path);

#endif  // BUILD_H