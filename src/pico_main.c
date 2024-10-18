#include "libpipam/common.h"
#include "libpipam/config.h"
#include "libpipam/db.h"
#include "libpipam/log.h"
#include "libpipam/memfb.h"
#include "libpipam/pkg.h"
#include "libpipam/tar.h"
#include "libpipam/types.h"
#include "libpipam/xfunc.h"

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int pkg_install_proto(const char* pkg_filepath)
{
  pkg_t*     pkg;
  archive_t* a;
  memfb_t*   a_pkg_data;
  int        r = EXIT_SUCCESS;

#if PICO_ROOT_CHECK == 1
  if (!is_root()) {
    pico_log_die(LOG_INFO, "This action requires root");
  }
#endif

  if (!fexists(pkg_filepath)) {
    pico_log_die(LOG_ERROR, "%s: No such file or directory", pkg_filepath);
  }

  a                     = tar_open_filename(pkg_filepath);

  memfb_t* tar_pkg_data = tar_read_file(a, "pico.dat");

  if (!tar_pkg_data) {
    tar_free(a);
    pico_log_die(LOG_ERROR, "%s: Not a package", pkg_filepath);
  }

  pkg = pkg_init();

  // TODO: USE STATE TO TRACK POSSIBLE ERRORS OCCURED DUE TO INITIALIZATION
  int ir = pkg_init_FILE(tar_pkg_data->fp, &pkg);
  if (ir != EXIT_SUCCESS) {
    r = EXIT_FAILURE;
    goto exit;
  }

  pico_log(LOG_INFO, "Installing %s:%s (%s)", pkg->name, pkg->arch, pkg->version);

  int pr = pkg_solve_depends(pkg);
  if (pr != EXIT_SUCCESS) {
    r = EXIT_FAILURE;
    goto exit;
  }

  int tr = tar_extract_to_prefix(a, &pkg->files, PICO_PREFIX);
  if (tr != EXIT_SUCCESS) {
    r = EXIT_FAILURE;
    goto exit;
  }

  int wr = db_write_pkg(pkg);
  if (wr != EXIT_SUCCESS) {
    r = EXIT_FAILURE;
    goto exit;
  }

  pico_log(LOG_INFO, "Installed %s:%s (%s)", pkg->name, pkg->arch, pkg->version);

exit:
  pkg_free(pkg);
  mem_fb_free(tar_pkg_data);
  tar_free(a);
  return EXIT_SUCCESS;
}

int pkg_remove_proto(const char* name)
{
  pkg_t* pkg;
  int    r = EXIT_SUCCESS;
  int    rr;

#if PICO_ROOT_CHECK == 1
  if (!is_root()) {
    pico_log_die(LOG_INFO, "This action requires root");
  }
#endif

  if (!db_exists_pkgname(name)) {
    pico_log_die(LOG_INFO, "%s is not installed", name);
  }

  pkg    = pkg_init();

  int pr = pkg_init_db(&pkg, name);
  if (pr != EXIT_SUCCESS) {
    r = EXIT_FAILURE;
    goto exit;
  }

  pico_log(LOG_INFO, "Trying to remove %s:%s (%s)", pkg->name, pkg->arch, pkg->version);

  int dr = pkg_solve_removal_depends(pkg);
  if (dr != EXIT_SUCCESS) {
    r = EXIT_FAILURE;
    goto exit;
  }

  for (size_t lv = 0; lv < PICO_MAX_REMLEVEL; lv++) {
    for (size_t i = 0; i < pkg->files->len; i++) {
      const char* filepath = vec_get(pkg->files, i);
      if(!fexists(filepath))
        continue;

      if (fislnk(filepath)) {
        rr = unlink(filepath);
      }
      else if(fisreg(filepath)) {
        rr = remove(filepath);
      }

      if (rr != EXIT_SUCCESS && (errno != ENOENT || errno != ENOTEMPTY) ) {
        r = EXIT_FAILURE;
        pico_log(LOG_WARN, "Unable to remove file %s: %s", filepath, strerror(errno));
      }
    }
  }
  
  if (r != EXIT_SUCCESS) {
    goto exit;
  }
  db_remove_pkg(pkg);
  pico_log(LOG_INFO, "Removed %s:%s (%s)", pkg->name, pkg->arch, pkg->version);

exit:
  pkg_free(pkg);
  return r;
}

int db_pkg_list()
{
  struct dirent* entry;

  pkg_t*         installed_pkg;
  FILE*          db_fp            = NULL;
  DIR*           db_dir           = NULL;
  size_t         db_pkg_path_size = 0;
  char*          db_pkg_path      = NULL;

  db_dir                          = xopendir(PICO_DB_PATH);

  while ((entry = readdir(db_dir)) != NULL) {
    const char* de_path = entry->d_name;
    if (entry->d_type == DT_DIR) {
      continue;
    }

    db_pkg_path_size = strlen(de_path) + strlen(PICO_DB_PATH) + strlen("/") + 1;
    db_pkg_path      = (char*)xmalloc(db_pkg_path_size);
    sprintf(db_pkg_path, PICO_DB_PATH "/%s", de_path);

    db_fp         = xfopen(db_pkg_path, "r");

    installed_pkg = pkg_init();
    pkg_init_FILE(db_fp, &installed_pkg);

    printf("%s:%s %s\n", installed_pkg->name, installed_pkg->arch, installed_pkg->version);

    pkg_free(installed_pkg);
    fclose(db_fp);
    xfree(db_pkg_path);
  }
  closedir(db_dir);
  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  if (!argv[1] || argv[1][0] != '-' || !argv[1][1])
    return EXIT_FAILURE;
  switch (argv[1][1]) {
  case 'i': {
    if (argv[2])
      return pkg_install_proto(argv[2]);
  } break;
  case 'r': {
    if (argv[2])
      return pkg_remove_proto(argv[2]);
  } break;
  case 'l':
    return db_pkg_list();
  default:
    return EXIT_SUCCESS;
  }
}