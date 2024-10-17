/*
 * Sweet tarballs (what? O_o)
 * Paul Goldstein, Oct 2024
 */
#pragma once
#include <archive.h>
#include <archive_entry.h>

#include "memfb.h"
#include "types.h"

#ifndef TAR_LIBARCHIVE_FILENAME_BUFFER
#define TAR_LIBARCHIVE_FILENAME_BUFFER 10240
#endif

#ifndef TAR_ENABLE_LZMA
#define TAR_ENABLE_LZMA 1
#endif

#ifndef TAR_ENABLE_GZIP
#define TAR_ENABLE_GZIP 0
#endif

typedef struct archive archive_t;
typedef struct archive_entry archive_entry_t;

archive_t *tar_open_filename(const char *filepath);
archive_t *tar_open_FILE(FILE *fp);

void tar_free(archive_t *a);

memfb_t* tar_read_file(archive_t* a, const char* te_path);
int tar_extract_to_prefix(archive_t* a, vec_t** files, const char* prefix);