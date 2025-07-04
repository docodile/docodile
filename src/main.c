#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ANSI-color-codes.h"
#include "builder.h"
#include "html.h"
#include "lex.h"
#include "parser.h"
#include "search.h"
#include "server.h"
#include "template.h"

#define BUILDDIR ".site"

char *ShiftArg(char ***argv);
void Help();
void New();

int main(int argc, char **argv) {
  ShiftArg(&argv);  // Program name
  char *command = ShiftArg(&argv);
  if (!command) command = "help";

  if (strcmp("help", command) == 0) {
    Help();
    return 0;
  }

  if (strcmp("new", command) == 0) {
    New();
    return 0;
  }

  if (strcmp("build", command) == 0) {
    LoadConfig();
    Directory *site_directory = NewDirectory("");
    BuildSiteDirectory(site_directory, DOCSDIR, 0);
    Build404Page(site_directory, DOCSDIR "/404.md");
    SortDirectory(site_directory);
    InitializeSite("site");
    SearchIndex index = BuildSearchIndex(site_directory);
    WriteSearchIndex(index, "site/assets/search.json");
    FreeSearchIndex(index);
    BuildSite(site_directory, site_directory, "site");
    UnloadConfig();
    return 0;
  }

  if (strcmp("serve", command) == 0) {
    Serve(BUILDDIR);
  }

  return 0;
}

char *ShiftArg(char ***argv) { return *(*argv)++; }

// ╚╔╩╦╠═╬╣║╝╗
void Help() {
  printf(
      "Usage: docodile [OPTIONS] COMMAND [ARGS]...\n"
      "\n");

  // printf(GRN
  //        "╔╗╔╗╔╗╔╣╔╗╔╗\n"
  //        "║╗╠╝║║║║║║║ \n"
  //        "╚╝╚╝╝╚╚╝╚╝╚╝\n" CRESET);

  // printf(GRN
  //        "╔╗╔╗╔╗╔╣╔╗╔╗\n"
  //        "╚╣╠╝║║║║║║║ \n"
  //        "╚╝╚╝╝╚╚╝╚╝╚╝\n" CRESET);

  // printf(GRN
  //        "╔═╗╔═╗╔═╗╔═╣╔═╗╔═╗\n"
  //        "╚═╣╠═╝║ ║║ ║║ ║║  \n"
  //        "╚═╝╚═╝╝ ╚╚═╝╚═╝╚═╝\n" CRESET);

  printf(HBLK
         "╔═ Options ═══════════════════════════════════╗\n"
         "║ -V, --version   Show the version and exit.  ║\n"
         "║ -h, --help      Show this message and exit. ║\n"
         "╚═════════════════════════════════════════════╝\n" CRESET);

  printf(BLU
         "╔═ Commands ══════════════════════╗\n"
         "║ build   Build the static site.  ║\n"
         "║ serve   Run development server. ║\n"
         "║ new     Create a new project.   ║\n"
         "╚═════════════════════════════════╝\n" CRESET);
}

void New() {
  // HACK Being lazy, will come back to this and implement properly.
  system("cp -r /etc/docodile/* ./");
  system("rm docodile install.sh");
}