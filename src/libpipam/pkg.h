/*
 * The Package. The Values. The Dependencies. The Conflicts
 * Paul Goldstein, Oct 2024
 */
#pragma once

#include <stddef.h>
#include <stdio.h>

#include "types.h"
#include "xfunc.h"

typedef struct pkg pkg_t;

enum pkg_states {
  STATE_FAILED           = 0b00000001,
  STATE_NO_VALUE_NAME    = 0b00000010,
  STATE_NO_VALUE_ARCH    = 0b00000100,
  STATE_NO_VALUE_VERSION = 0b00001000,
  STATE_WRONG_ARCH_CHECK = 0b00010000,
  STATE_NO_DEPS_CHECK    = 0b00100000,
  STATE_NO_CONFS_CHECK   = 0b01000000,
  STATE_NO_FILES_CHECK   = 0b10000000,
};

struct pkg {
  char * name, *arch, *version;

  vec_t *deps, *confs;

  vec_t* files;

  int    state;
};

pkg_t* pkg_init();
void   pkg_free(pkg_t* pkg);

int    pkg_init_FILE(FILE* fp, pkg_t** pkg);
int    pkg_parse_FILE(FILE* fp, pkg_t* pkg);
int    pkg_write_FILE(FILE* fp, pkg_t* pkg);

int    pkg_check_values(pkg_t* pkg);
int    pkg_check_arch(pkg_t* pkg);

int    pkg_solve_depends(pkg_t* pkg);
int    pkg_solve_conflicts(pkg_t* pkg);
int    pkg_solve_removal_depends(pkg_t* pkg);

// Unused due to mem_fb_t existance.
// void pkg_init_filepath(const char *fp);

// Debugging function, marked for deletion.
void pkg_print(pkg_t* pkg);

void pkg_free(pkg_t* pkg);