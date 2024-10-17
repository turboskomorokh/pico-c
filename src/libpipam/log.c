#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

void pico_log(enum log_level l, const char *f, ...) {
  const char *log_str = "";
  va_list args;
  FILE *out_stream;

  switch (l) {
  case LOG_INFO: {
    log_str = "";
    out_stream = stdout;
  } break;

  case LOG_WARN: {
    log_str = "Warning: ";
    out_stream = stdout;
  } break;

  case LOG_ERROR: {
    log_str = "Error: ";
    out_stream = stderr;
  } break;

  case LOG_DEBUG: {
    log_str = "DEBUG: ";
    out_stream = stderr;
  } break;

  default: {
    out_stream = stdout;
  } break;
  }
  fprintf(out_stream, "%s", log_str);
  
  va_start(args, f);
  vfprintf(out_stream, f, args);
  va_end(args);
  
  fputc('\n', out_stream);
  
  return;
}

void pico_log_die(enum log_level l, const char *f, ...) {
  const char *log_str;
  va_list args;
  FILE *out_stream;

  switch (l) {
  case LOG_INFO: {
    log_str = "";
    out_stream = stdout;
  } break;

  case LOG_WARN: {
    log_str = "Warning: ";
    out_stream = stdout;
  } break;

  case LOG_ERROR: {
    log_str = "Error: ";
    out_stream = stderr;
  } break;

  case LOG_DEBUG: {
    log_str = "DEBUG: ";
    out_stream = stderr;
  } break;

  default: {
    log_str = "";
    out_stream = stdout;
  } break;
  }

  va_start(args, f);
  vfprintf(out_stream, f, args);
  va_end(args);
  

  fputc('\n', out_stream);

  exit(EXIT_SUCCESS);

  return;
}
