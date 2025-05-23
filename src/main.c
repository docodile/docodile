#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "builder.h"
#include "html.h"
#include "lex.h"
#include "logger.h"
#include "parser.h"
#include "server.h"
#include "template.h"

#define BUILDDIR "site"

int main(void) {
  Directory *site_directory = NewDirectory("");
  BuildSiteDirectory(site_directory, DOCSDIR);
  InitializeSite(BUILDDIR);
  Nav *nav = BuildNav(site_directory);
  BuildSite(site_directory, BUILDDIR, nav);

  Serve(BUILDDIR);

  return 0;
}