/*
 * Memory File Buffer. Used in tar file-to-memory stuff
 * Paul Goldstein, Oct 2024
*/

#pragma once

#include <stdio.h>

typedef struct mem_file_buffer memfb_t;

struct mem_file_buffer
{
  void *buf;
  FILE *fp;
};

memfb_t* mem_fb_new(size_t s);
void mem_fb_free(memfb_t* mfb);
