#include <archive.h>
#include <archive_entry.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "memfb.h"
#include "tar.h"
#include "types.h"
#include "xfunc.h"

void tar_free(archive_t* a)
{
  archive_read_free(a);
  return;
}

void tar_archive_set_support(archive_t* a)
{
  archive_read_support_format_tar(a);

#if TAR_ENABLE_LZMA == 1
  archive_read_support_filter_lzma(a);
#endif
#if TAR_ENABLE_GZIP == 1
  archive_read_support_filter_gzip(a);
#endif
  return;
}

archive_t* tar_open_filename(const char* filename)
{
  archive_t* a = archive_read_new();

  tar_archive_set_support(a);

  archive_read_open_filename(a, filename, TAR_LIBARCHIVE_FILENAME_BUFFER);
  return a;
}

archive_t* tar_open_FILE(FILE* fp)
{
  archive_t* a = archive_read_new();

  tar_archive_set_support(a);

  archive_read_open_FILE(a, fp);

  return a;
}

memfb_t* tar_read_file(archive_t* a, const char* te_path)
{
  archive_entry_t* ae;
  size_t           ae_size = 0, bytes_read = 0;
  int              exists = 0;
  memfb_t*         mfb    = NULL;

  if (!a) {
    pico_log_die(LOG_ERROR, "%s(): archive is NULL", __func__);
  }

  while (archive_read_next_header(a, &ae) == ARCHIVE_OK) {
    const char* ae_path = archive_entry_pathname(ae);
    if (!strncmp(ae_path, te_path, strlen(te_path))) {
      pico_log(LOG_DEBUG, "Found requested %s in archive\n", te_path);
      ae_size    = archive_entry_size(ae);
      mfb        = mem_fb_new(ae_size);
      bytes_read = archive_read_data(a, mfb->buf, ae_size);
      if (bytes_read != ae_size) {
        pico_log(LOG_ERROR, "%s(): failed to read data for %s", __func__, te_path);
        mem_fb_free(mfb);
        goto end;
      }
      exists = 1;
      break;
    }
  }
  if (!exists) {
    #if DEBUG == 1
    pico_log(LOG_ERROR, "%s(): file %s isn't found in archive", __func__, te_path);
    #endif
  }
end:
  return mfb;
}

int tar_extract_to_prefix(archive_t* a, vec_t** files, const char* prefix)
{
  char*            ex_path      = NULL;
  size_t           ex_path_size = 0, bytes_read = 0;
  archive_entry_t* ae;
  int              r = EXIT_SUCCESS;

  if (!a) {
    pico_log_die(LOG_ERROR, "%s(): archive is NULL", __func__);
  }

  if (!*files) {
    *files = vec_init();
  }

  while (archive_read_next_header(a, &ae) == ARCHIVE_OK) {
    const char* ae_path = archive_entry_pathname(ae);
    if(!strcmp(ae_path, "pico.dat"))
      continue;

    ex_path_size        = strlen(prefix) + strlen(ae_path) + strlen("/") + 1;
    ex_path             = xmalloc(ex_path_size);
    sprintf(ex_path, "%s/%s", prefix, ae_path);

    archive_entry_set_pathname(ae, ex_path);
    if (archive_read_extract(a, ae, 0)) {
      pico_log(LOG_WARN, "%s(): Unable to to extract %s: %s", __func__, ex_path, archive_error_string(a));
      r = EXIT_FAILURE;
      goto extract_next;
    }

    vec_push_back(*files, ex_path);

  extract_next:
    xfree(ex_path);
  }

end:
  return r;
}