/*
 * Folder database my beloved.
 * This idea was fire, my workspace agreed.
 * Paul Goldstein, Oct 2024
 */

#pragma once

#include "pkg.h"
#include "types.h"

int pkg_init_db(pkg_t** pkg, const char* name);

int db_write_pkg(pkg_t* pkg);

int db_exists_pkgname(const char *name);
int db_exists_pkg(pkg_t *pkg);

int db_check_pkgs_conflict(vec_t* conflict_list, const char* name);
int db_check_pkgs_dependant(vec_t* depends_list, const char* name);

int db_remove_pkgname(const char* name);
int db_remove_pkg(pkg_t* pkg);
