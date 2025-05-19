#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
  const char *color_scheme;
  const char *accent_color;
} Theme;

typedef struct {
  Theme theme;
} Config;

Config LoadConfig();

#endif  // CONFIG_H