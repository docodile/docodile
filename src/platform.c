#include "platform.h"

#define CMDBUFFER 1000

static bool LinuxCommandExists(const char *cmd) {
  char command[CMDBUFFER];
  sprintf(command, "command -v %s > /dev/null 2>&1", cmd);
  return system(command) == 0;
}

void OpenBrowser(const char *url) {
#if defined(_WIN32) || defined(_WIN64)
  char command[CMDBUFFER];
  sprintf(command, "start %s", url);
  system(command);
#elif defined(__APPLE__)
  char command[CMDBUFFER];
  sprintf(command, "open \"%s\"", url);
  system(command);
#elif defined(__linux__)
  if (LinuxCommandExists("xdg-open")) {
    char command[CMDBUFFER];
    sprintf(command, "xdg-open \"%s\"", url);
    system(command);
  } else if (LinuxCommandExists("gnome-open")) {
    char command[CMDBUFFER];
    sprintf(command, "gnome-open \"%s\"", url);
    system(command);
  } else if (LinuxCommandExists("kde-open")) {
    char command[CMDBUFFER];
    sprintf(command, "kde-open \"%s\"", url);
    system(command);
  } else {
    fprintf(stderr, "Unable to open browser.");
  }
#else
  fprintf(stderr, "Platform not recognized. Unable to open browser.");
#endif
}