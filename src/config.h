#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
  const char *color_scheme;
  const char *accent_color;
} Theme;

typedef struct {
  Theme theme;
} Config;

#define MAXVALUES      100
#define MAXSECTIONNAME 100
#define MAXKEY         100
#define MAXVALUE       100

void LoadConfig();
void UnloadConfig();
char *ReadConfig(const char *path);

#endif  // CONFIG_H