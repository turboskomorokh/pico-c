#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/limits.h>

#include "arch.h"
#include "db.h"
#include "log.h"
#include "parse.h"
#include "pkg.h"
#include "types.h"
#include "xfunc.h"

pkg_t* pkg_init()
{
  pkg_t* pkg   = xmalloc(sizeof(pkg_t));
  pkg->name    = NULL;
  pkg->arch    = NULL;
  pkg->version = NULL;
  pkg->deps    = NULL;
  pkg->confs   = NULL;
  pkg->state   = 0;
  return pkg;
}

void pkg_free(pkg_t* pkg)
{
  if (pkg) {
    if (pkg->name)
      xfree(pkg->name);
    if (pkg->arch)
      xfree(pkg->arch);
    if (pkg->version)
      xfree(pkg->version);
    if (pkg->deps)
      vec_free(pkg->deps);
    if (pkg->confs)
      vec_free(pkg->confs);
    if (pkg->files)
      vec_free(pkg->files);
    xfree(pkg);
  }
  return;
}

void pkg_print(pkg_t* pkg)
{
#if DEBUG == 1
  printf("State: %016x\n", pkg->state);

  printf("Name: '%s'\n", pkg->name);
  printf("Arch: '%s'\n", pkg->arch);
  printf("Version: '%s'", pkg->version);
  printf("\nDepends: ");
  for (int i = 0; i < pkg->deps->len; i++) {
    printf("'%s', ", pkg->deps->data[i]);
  }
  printf("\nConflicts: ");
  for (int i = 0; i < pkg->confs->len; i++) {
    printf("'%s',", pkg->confs->data[i]);
  }
  printf("\nFiles: \n");
  for (int i = 0; i < pkg->files->len; i++) {
    printf("* '%s'\n", pkg->files->data[i]);
  }
#endif

  return;
}

int pkg_init_FILE(FILE* fp, pkg_t** pkg)
{
  if (!fp) {
    pico_log_die(LOG_ERROR, "%s(): bad file pointer", __func__);
  }
  if (!*pkg) {
    *pkg = pkg_init();
  }

  xfseek(fp, SEEK_SET, 0);

  return pkg_parse_FILE(fp, *pkg);
}

int pkg_write_FILE(FILE* fp, pkg_t* pkg)
{
  if (!fp) {
    pico_log_die(LOG_ERROR, "%s(): bad file pointer", __func__);
  }
  if (!pkg) {
    pico_log_die(LOG_ERROR, "%s(): pkg_t is NULL", __func__);
  }

  fprintf(fp,
      "package: %s\n"
      "arch: %s\n"
      "version: %s",
      pkg->name, pkg->arch, pkg->version);
  fprintf(fp, "\ndepends: ");
  for (size_t i = 0; i < pkg->deps->len; i++) {
    fprintf(fp, "%s,", vec_get(pkg->deps, i));
  }
  fprintf(fp, "\nconflicts: ");

  for (size_t i = 0; i < pkg->confs->len; i++) {
    fprintf(fp, "%s,", vec_get(pkg->confs, i));
  }

  fprintf(fp, "\nfiles: ");
  for (size_t i = 0; i < pkg->files->len; i++) {
    fprintf(fp, "::%s", vec_get(pkg->files, i));
  }

  return EXIT_SUCCESS;
}

int pkg_parse_FILE(FILE* fp, pkg_t* pkg)
{
  for (char line[PATH_MAX]; fgets(line, PATH_MAX, fp);) {
    switch (line[0]) {
    case 'a': {
      if (parse_has_type(line, "arch"))
        pkg->arch = parse_value(line, "arch", ": ");
    } break;
    case 'c': {
      if (parse_has_type(line, "conflicts"))
        pkg->confs = parse_vec(line, "conflicts", ": ", ",");
    } break;
    case 'd': {
      if (parse_has_type(line, "depends"))
        pkg->deps = parse_vec(line, "depends", ": ", ",");
    } break;
    case 'p': {
      if (parse_has_type(line, "package"))
        pkg->name = parse_value(line, "package", ": ");
    } break;
    case 'v': {
      if (parse_has_type(line, "version"))
        pkg->version = parse_value(line, "version", ": ");
    } break;
    case 'f': {
      if (parse_has_type(line, "files"))
        pkg->files = parse_vec(line, "files", ": ", "::");
    } break;
    }
  }

  if (!pkg->deps)
    pkg->deps = vec_init();
  if (!pkg->confs)
    pkg->confs = vec_init();
  if (!pkg->files)
    pkg->files = vec_init();

  return pkg_check_values(pkg);
}

int pkg_check_values(pkg_t* pkg)
{
  pkg->state |= !strlen(pkg->name) ? (STATE_FAILED | STATE_NO_VALUE_NAME) : 0; // 0b00000011;
  pkg->state |= !strlen(pkg->version) ? (STATE_FAILED | STATE_NO_VALUE_VERSION) : 0; // 0b00000101;
  pkg->state |= !strlen(pkg->arch) ? (STATE_FAILED | STATE_NO_VALUE_ARCH) : 0; // 0b00001001;

  if (pkg->state & STATE_FAILED) {
    int st = pkg->state & ~STATE_FAILED;
    switch (st) {
    case STATE_NO_VALUE_NAME: {
      pico_log(LOG_ERROR, "%s(): package doesn't have a name!\n", __func__);
    } break;
    case STATE_NO_VALUE_ARCH: {
      pico_log(LOG_ERROR, "%s(): package doesn't have an arch!\n", __func__);
    } break;
    case STATE_NO_VALUE_VERSION: {
      pico_log(LOG_ERROR, "%s(): package doesn't have a version!\n", __func__);
    } break;
    default:
      break;
    }
    return EXIT_FAILURE;
  }

  pkg->state |= (!pkg->deps->len ? STATE_NO_DEPS_CHECK : 0);
  pkg->state |= (!pkg->confs->len ? STATE_NO_CONFS_CHECK : 0);

  pkg->state |= (!pkg->files->len ? STATE_NO_FILES_CHECK : 0);
  return pkg_check_arch(pkg);
}

int pkg_check_arch(pkg_t* pkg)
{
  if (!strcmp(pkg->arch, "none"))
    goto end;

  if (!cmp_arch(pkg->arch)) {
    pico_log(LOG_WARN, "Package %s is incompatible with host architecture (arch: %s)", pkg->name, pkg->arch);
    pkg->state |= STATE_WRONG_ARCH_CHECK;
    return EXIT_FAILURE;
  }

end:
  return EXIT_SUCCESS;
}

int pkg_solve_depends(pkg_t* pkg)
{
  if (!pkg) {
    pico_log_die(LOG_ERROR, "%s(): Unable to solve dependencies: pkg_t is NULL", __func__);
  }

  if (pkg->state & STATE_NO_DEPS_CHECK) {
    goto next_check;
  }

  vec_t* failed_depends = vec_init();

  for (size_t i = 0; i < pkg->deps->len; i++) {
    const char* dep = vec_get(pkg->deps, i);
    if (!db_exists_pkgname(dep)) {
      vec_push_back(failed_depends, dep);
    }
  }

  if (failed_depends->len) {
    pkg->state |= STATE_FAILED;
    pico_log(LOG_INFO, "Package %s depends on:", pkg->name);
    for (size_t i = 0; i < failed_depends->len; i++) {
      const char* failed_dep = vec_get(failed_depends, i);
      pico_log(LOG_INFO, " * %s", failed_dep);
    }
    pico_log(LOG_INFO, "Consider installing these first");
  }

  vec_free(failed_depends);

next_check:
  return pkg_solve_conflicts(pkg);
}

int pkg_solve_conflicts(pkg_t* pkg)
{
  vec_t *personal_conflicts, *installed_conflicts;
  int    r = EXIT_SUCCESS;

  if (!(pkg->state & STATE_NO_CONFS_CHECK))
    goto check_installed_conflicts;

  personal_conflicts = vec_init();

  for (size_t i = 0; i < pkg->confs->len; i++) {
    const char* conflict = vec_get(pkg->confs, i);
    if (db_exists_pkgname(conflict))
      vec_push_back(personal_conflicts, conflict);
  }

  if (personal_conflicts->len) {
    pkg->state |= STATE_FAILED;
    pico_log(LOG_INFO, "Package %s conflicts with installed packages:", pkg->name);
    for (size_t i = 0; i < personal_conflicts->len; i++) {
      const char* personal_conflict = vec_get(personal_conflicts, i);
      pico_log(LOG_INFO, " * %s", personal_conflict);
    }
    pico_log(LOG_INFO, "Consider removing these first");
    r = EXIT_FAILURE;
  }

  vec_free(personal_conflicts);

check_installed_conflicts:

  installed_conflicts = vec_init();

  if (db_check_pkgs_conflict(installed_conflicts, pkg->name) != EXIT_SUCCESS) {
    pkg->state |= STATE_FAILED;
    pico_log(LOG_INFO, "These installed packages conflict with %s", pkg->name);
    for (size_t i = 0; i < installed_conflicts->len; i++) {
      const char* installed_conflict = vec_get(installed_conflicts, i);
      pico_log(LOG_INFO, " * %s", installed_conflict);
    }
    r = EXIT_FAILURE;
  }

  vec_free(installed_conflicts);

next_check:
  return r;
}

int pkg_solve_removal_depends(pkg_t* pkg)
{
  vec_t* dependant_pkgs;
  int    r       = EXIT_SUCCESS;

  dependant_pkgs = vec_init();

  int dr         = db_check_pkgs_dependant(dependant_pkgs, pkg->name);

  if (dr == EXIT_FAILURE) {
    pkg->state |= STATE_FAILED;
    pico_log(LOG_INFO, "These installed packages depends on %s", pkg->name);
    for (size_t i = 0; i < dependant_pkgs->len; i++) {
      const char* dependant = vec_get(dependant_pkgs, i);
      pico_log(LOG_INFO, " * %s", dependant);
    }
    r = EXIT_FAILURE;
  }

  vec_free(dependant_pkgs);

next_check:
  return r;
}