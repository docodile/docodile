#ifndef BUILDER_H
#define BUILDER_H

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "html.h"
#include "parser.h"
#include "template.h"
#include "utils.h"

#define DOCSDIR        "docs"
#define DOCSMAXDEPTH   1000
#define MAXDIRS        1000
#define MAXPAGESPERDIR 1000
#define MAXFILENAMELEN 1000
#define MAXFILEPATH    1000

typedef struct {
  char out_name[MAXFILENAMELEN];
  char src_name[MAXFILENAMELEN];
  char full_path[MAXFILEPATH];
} Page;

Page *NewPage(const char *name, const char *fullpath);

typedef struct Directory {
  char path[MAXFILEPATH];
  bool hidden;
  size_t num_pages;
  size_t num_dirs;
  Page *pages[MAXPAGESPERDIR];
  struct Directory *dirs[MAXDIRS];
} Directory;

Directory *NewDirectory(const char *path);
void FreeDirectory(Directory *dir);

void BuildSiteDirectory(Directory *dest, const char *path);
void InitializeSite(const char *dir);
void BuildSite(Directory *site_directory, const char *base_path, Nav *nav);
Nav *BuildNav(Directory *dir);

#endif  // BUILD_H