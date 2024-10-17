#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"

#if DEBUG == 1
#include "log.h"
#include <errno.h>
#include <string.h>
#endif

int fexists(const char* p)
{
  struct stat st;
  if (stat(p, &st) == EXIT_SUCCESS) {
    return 1;
  }
#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s: %s:%s", __func__, p, strerror(errno));
#endif
  return 0;
}

int is_root() {
  return getuid() == 0;
}