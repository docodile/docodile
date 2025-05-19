#include "config.h"

// TODO load from a file
Config LoadConfig() {
  return (Config){.theme = {.accent_color = "red", .color_scheme = "dark"}};
}