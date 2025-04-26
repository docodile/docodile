#include <stdio.h>
#include <time.h>
#include <unistd.h>

#ifndef LOGGER_H
#define LOGGER_H

#define LOG_LEVEL_ERR   0
#define LOG_LEVEL_WARN  1
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_TRACE 3
#define LOG_LEVEL_DEBUG 4

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif  // LOG_LEVEL

#define _RED "\033[31m"
#define _YLW "\033[33m"
#define _BLU "\033[34m"
#define _CYN "\033[36m"
#define _GRY "\033[90m"
#define _RST "\033[0m"

#define _LBL_ERR   "[ERROR] "
#define _LBL_WARN  "[WARN]  "
#define _LBL_INFO  "[INFO]  "
#define _LBL_TRACE "[TRACE] "
#define _LBL_DEBUG "[DEBUG] "

#define _FMT(color, label, fmt) color label fmt _RST "\n"

static void get_current_time(char formatted_time[20]) {
  time_t now            = time(NULL);
  struct tm *local_time = localtime(&now);
  strftime(formatted_time, 20, "%Y-%m-%d %H:%M:%S", local_time);
}

#define LOG(stream, fmt, ...)                                                      \
  {                                                                                \
    char ftime[20];                                                                \
    get_current_time(ftime);                                                       \
    fprintf(stream, _GRY "%s "_RST fmt, ftime, ##__VA_ARGS__, __FILE__, __LINE__); \
  }

#if LOG_LEVEL >= LOG_LEVEL_ERR
#define LOG_ERR(fmt, ...) LOG(stderr, _FMT(_RED, _LBL_ERR, fmt " (%s:%d)"), ##__VA_ARGS__)
#else
#define LOG_ERR(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(fmt, ...) LOG(stdout, _FMT(_YLW, _LBL_WARN, fmt), ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(fmt, ...) LOG(stdout, _FMT(_BLU, _LBL_INFO, fmt), ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_TRACE
#define LOG_TRACE(fmt, ...) LOG(stdout, _FMT(_CYN, _LBL_TRACE, fmt), ##__VA_ARGS__)
#else
#define LOG_TRACE(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#undef LOG_DEBUG
#define LOG_DEBUG(fmt, ...) LOG(stdout, _FMT(_GRY, _LBL_DEBUG, fmt), ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#endif  // LOGGER_H