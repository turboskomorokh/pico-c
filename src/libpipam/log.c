#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

void pico_logger(enum log_level l, const char* f, va_list* args)
{
  const char* log_str = "";
  FILE*       out_stream;

  switch (l) {
  case LOG_INFO: {
    out_stream = stdout;
  } break;

  case LOG_WARN: {
    log_str    = "[\e[0;33mWarn\e[0m]: ";
    out_stream = stdout;
  } break;

  case LOG_ERROR: {
    log_str    = "[\e[1;30mError\e[0m]: ";
    out_stream = stderr;
  } break;

  case LOG_DEBUG: {
    log_str    = "\e[1;70mDEBUG\e[0m]: ";
    out_stream = stderr;
  } break;

  default: {
    out_stream = stdout;
  } break;
  }
  fprintf(out_stream, "%s", log_str);
  vfprintf(out_stream, f, *args);
  fputc('\n', out_stream);
}

void pico_log(enum log_level l, const char* f, ...)
{
  va_list args;

  va_start(args, f);

  pico_logger(l, f, &args);

  va_end(args);

  return;
}

void pico_log_die(enum log_level l, const char* f, ...)
{
  va_list args;

  va_start(args, f);

  pico_logger(l, f, &args);
  
  va_end(args);

  exit(EXIT_SUCCESS);

  return;
}