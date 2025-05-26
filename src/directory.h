#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAXDIRS        1000
#define MAXPAGESPERDIR 1000
#define MAXFILENAMELEN 1000
#define MAXFILEPATH    1000
#define MAXURL         1000

typedef struct {
  char out_name[MAXFILENAMELEN];
  char src_name[MAXFILENAMELEN];
  char full_path[MAXFILEPATH];
  char url[MAXURL];
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

#endif  // DIRECTORY_H