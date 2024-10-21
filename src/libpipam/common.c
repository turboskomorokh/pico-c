#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"

#if DEBUG == 1
#include "log.h"
#include <errno.h>
#include <string.h>
#endif

struct stat fst;

int fexists(const char* p)
{
  if (stat(p, &fst) == EXIT_SUCCESS) {
    return 1;
  }
#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s: %s:%s", __func__, p, strerror(errno));
#endif
  return 0;
}

int flexists(const char* p)
{
  if (lstat(p, &fst) == EXIT_SUCCESS) {
    return 1;
  }
#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s: %s:%s", __func__, p, strerror(errno));
#endif
  return 0;
}

int fisreg(const char *p) {
  if (lstat(p, &fst) == EXIT_SUCCESS) {
    return S_ISREG(fst.st_mode);
  }
#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s: %s:%s", __func__, p, strerror(errno));
#endif
  return 0;
}
int fisdir(const char *p) {
  if (lstat(p, &fst) == EXIT_SUCCESS) {
    return S_ISDIR(fst.st_mode);
  }
#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s: %s:%s", __func__, p, strerror(errno));
#endif
  return 0;
}

int fislnk(const char *p){
  if (lstat(p, &fst) == EXIT_SUCCESS) {
    return S_ISLNK(fst.st_mode);
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