/*
 * Necessary logging functions
 * (THEY ARE)
 * Paul Goldstein, Oct 2024
 */

#pragma once

enum log_level {
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_DEBUG,
};

void pico_log(enum log_level, const char *f, ...);
void pico_log_die(enum log_level, const char *f, ...);