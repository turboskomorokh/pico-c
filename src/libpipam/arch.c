#include <string.h>
#include <sys/utsname.h>

#include "log.h"

int cmp_arch(const char *t_arch) {
  struct utsname sys;
  int r = uname(&sys);
  if(r) {
    pico_log_die(LOG_ERROR, "%s(): uname() returned %d", __func__, r);
  }
  return strcmp(sys.machine, t_arch) == 0;
}