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
  if (lstat(p, &st) == EXIT_SUCCESS) {
    return 1;
  }
#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s: %s:%s", __func__, p, strerror(errno));
#endif
  return 0;
}

int fisreg(const char *p) {
  struct stat st;
  if (lstat(p, &st) == EXIT_SUCCESS) {
    return S_ISREG(st.st_mode);
  }
#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s: %s:%s", __func__, p, strerror(errno));
#endif
  return 0;
}
int fisdir(const char *p) {
  struct stat st;
  if (lstat(p, &st) == EXIT_SUCCESS) {
    return S_ISDIR(st.st_mode);
  }
#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s: %s:%s", __func__, p, strerror(errno));
#endif
  return 0;
}

int fislnk(const char *p){
  struct stat st;
  if (lstat(p, &st) == EXIT_SUCCESS) {
    return S_ISLNK(st.st_mode);
  }
#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s: %s:%s", __func__, p, strerror(errno));
#endif
  return 0;
}

int is_root() {
#if PICO_ROOT_CHECK == 1
  return 1;
#endif
  return getuid() == 0;
}