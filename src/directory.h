#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "utils.h"

#define MAXDIRS        1000
#define MAXPAGESPERDIR 1000
#define MAXFILENAMELEN 1000
#define MAXFILEPATH    1000
#define MAXURL         1000

typedef struct {
  int heading_level;
  char *link;
} TOCItem;

typedef struct {
  size_t count;
  TOCItem *items;
} TOC;

typedef struct Directory {
  char path[MAXFILEPATH];
  char title[MAXFILEPATH];
  bool hidden;
  size_t num_dirs;
  struct Directory *dirs[MAXDIRS];

  bool is_dir;
  bool is_index;

  char out_name[MAXFILENAMELEN];
  char src_name[MAXFILENAMELEN];
  char full_path[MAXFILEPATH];
  char url[MAXURL];
  char url_path[MAXURL];
  TOC toc;
} Directory;

typedef Directory Page;

Directory *NewDirectory(const char *path);
void SortDirectory(Directory *dir);
Page *NewPage(const char *name, const char *fullpath);
void FreeDirectory(Directory *dir);

#endif  // DIRECTORY_H