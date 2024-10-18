#include "db.h"
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "log.h"
#include "pkg.h"
#include "types.h"
#include "xfunc.h"

#include "config.h"

int db_write_pkg(pkg_t* pkg)
{
  FILE* db_fp = NULL;
  int   r     = EXIT_FAILURE;

  if (!pkg) {
    pico_log_die(LOG_ERROR, "%s(): Unable to write: pkg_t is NULL", __func__);
  }

  char* db_pkg_path = xmalloc(strlen(PICO_DB_PATH) + strlen(pkg->name) + strlen("/") + 1);
  sprintf(db_pkg_path, PICO_DB_PATH "/%s", pkg->name);

  db_fp = xfopen(db_pkg_path, "w");
  r     = pkg_write_FILE(db_fp, pkg);

  free(db_pkg_path);
  fclose(db_fp);

  return r;
}

int db_exists_pkgname(const char* name)
{
  char*  db_pkg_path = NULL;
  FILE*  db_fp       = NULL;
  pkg_t* pkg         = NULL;
  int    r           = 0;

  db_pkg_path        = xmalloc(strlen(PICO_DB_PATH) + strlen(name) + strlen("/") + 1);
  sprintf(db_pkg_path, PICO_DB_PATH "/%s", name);

  if (!fexists(db_pkg_path)) {
    xfree(db_pkg_path);
    return r;
  }

  db_fp = xfopen(db_pkg_path, "r");

  pkg_init_FILE(db_fp, &pkg);

  r = strcmp(pkg->name, name) == 0;

  pkg_free(pkg);
  fclose(db_fp);
  xfree(db_pkg_path);

  return r;
}

int db_exists_pkg(pkg_t* pkg)
{
  if (!pkg->name) {
    pico_log(LOG_WARN, "%s(): pkg->name value is NULL", __func__);
    return 0;
  }
  return db_exists_pkgname(pkg->name);
}

int pkg_init_db(pkg_t** pkg, const char* name)
{
  char* db_pkg_path = NULL;
  FILE* db_fp       = NULL;
  int   r           = EXIT_FAILURE;

  if (!db_exists_pkgname(name)) {
    pico_log(LOG_WARN, "%s(): %s isn't in database", __func__, name);
    return r;
  }

  db_pkg_path = xmalloc(strlen(PICO_DB_PATH) + strlen(name) + strlen("/") + 1);
  sprintf(db_pkg_path, PICO_DB_PATH "/%s", name);

  db_fp = xfopen(db_pkg_path, "r");

  r     = pkg_init_FILE(db_fp, pkg);

  fclose(db_fp);
  xfree(db_pkg_path);

  return r;
}

int db_remove_pkgname(const char* name)
{
  char* db_pkg_path = NULL;
  int   r           = EXIT_FAILURE;

  if (!name) {
    pico_log_die(LOG_WARN, "%s(): package name is NULL", __func__);
  }

  if (!db_exists_pkgname(name)) {
    pico_log(LOG_WARN, "%s(): %s isn't in database", __func__, name);
    return r;
  }

  db_pkg_path = xmalloc(strlen(PICO_DB_PATH) + strlen(name) + strlen("/") + 1);
  sprintf(db_pkg_path, PICO_DB_PATH "/%s", name);

  r = remove(db_pkg_path);

  xfree(db_pkg_path);
  return r;
}

int db_remove_pkg(pkg_t* pkg)
{
  if (!pkg->name) {
    pico_log_die(LOG_ERROR, "%s(): pkg->name value is NULL", __func__);
  }

  return db_remove_pkgname(pkg->name);
}

int db_check_pkgs_conflict(vec_t* conflict_list, const char* name)
{
  struct dirent* entry;

  pkg_t*         installed_pkg;
  FILE*          db_fp            = NULL;
  DIR*           db_dir           = NULL;
  size_t         db_pkg_path_size = 0;
  char*          db_pkg_path      = NULL;

  if (!conflict_list) {
    pico_log_die(LOG_WARN, "%s(): confs vector is NULL", __func__);
  }

  if (!name) {
    pico_log_die(LOG_WARN, "%s(): package name is NULL", __func__);
  }

  db_dir = xopendir(PICO_DB_PATH);

  while ((entry = readdir(db_dir)) != NULL) {
    const char* de_path = entry->d_name;
    if (entry->d_type == DT_DIR) {
      continue;
    }

    db_pkg_path_size = strlen(de_path) + strlen(PICO_DB_PATH) + strlen("/") + 1;
    db_pkg_path      = xmalloc(db_pkg_path_size);
    sprintf(db_pkg_path, PICO_DB_PATH "/%s", de_path);

    db_fp         = xfopen(db_pkg_path, "r");

    installed_pkg = pkg_init();
    pkg_init_FILE(db_fp, &installed_pkg);

    for (size_t i = 0; i < installed_pkg->confs->len; i++) {
      const char* conflict = installed_pkg->confs->data[i];
      if (!strcmp(conflict, name))
        vec_push_back(conflict_list, installed_pkg->name);
    }

    pkg_free(installed_pkg);

    fclose(db_fp);
    free(db_pkg_path);
  }
  closedir(db_dir);
  return (conflict_list)->len;
}

int db_check_pkgs_dependant(vec_t* depends_list, const char* name)
{
  struct dirent* entry;

  pkg_t*         installed_pkg    = NULL;
  FILE*          db_fp            = NULL;
  DIR*           db_dir           = NULL;
  size_t         db_pkg_path_size = 0;
  char*          db_pkg_path      = NULL;

  if (!depends_list) {
    pico_log_die(LOG_WARN, "%s(): depends vector is NULL", __func__);
  }

  if (!name) {
    pico_log_die(LOG_WARN, "%s(): package name is NULL", __func__);
  }

  db_dir = xopendir(PICO_DB_PATH);

  while ((entry = readdir(db_dir)) != NULL) {
    const char* de_path = entry->d_name;
    if (entry->d_type == DT_DIR || !strcmp(de_path, name)) {
      continue;
    }

    db_pkg_path_size = strlen(de_path) + strlen(PICO_DB_PATH) + strlen("/") + 1;
    db_pkg_path      = xmalloc(db_pkg_path_size);
    sprintf(db_pkg_path, PICO_DB_PATH "/%s", de_path);

    db_fp         = xfopen(db_pkg_path, "r");

    installed_pkg = pkg_init();

    pkg_init_FILE(db_fp, &installed_pkg);

#if DEBUG == 1
    pico_log(LOG_DEBUG, "Checking if %s depends on %s", installed_pkg->name, name);
#endif

    for (size_t i = 0; i < installed_pkg->deps->len; i++) {
      const char* dependency = vec_get(installed_pkg->deps, i);
      if (!strcmp(dependency, name)) {
        pico_log(LOG_DEBUG, "%s depends on %s", installed_pkg->name, name);
        vec_push_back(depends_list, installed_pkg->name);
      }
    }

    pkg_free(installed_pkg);

    fclose(db_fp);
    xfree(db_pkg_path);
  }
  closedir(db_dir);
  return (depends_list)->len ? EXIT_FAILURE : EXIT_SUCCESS;
}
