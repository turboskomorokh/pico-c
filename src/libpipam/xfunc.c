// Paul Goldstein, Oct 2024
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void xfree(void* ptr)
{
  if (ptr) {
    free(ptr);
    ptr = NULL;
  }
}

void* xmalloc(size_t s)
{
  void* p = malloc(s);
  if (!p) {
    perror("xmalloc");
    exit(EXIT_FAILURE);
  }
  return p;
}

void* xrealloc(void* p, size_t s)
{
  void* np = realloc(p, s);
  if (!np) {
    perror("xrealloc");
    exit(EXIT_FAILURE);
  }
  return np;
}

void* xcalloc(size_t nmemb, size_t s)
{
  void* p = calloc(nmemb, s);
  if (!p) {
    perror("xcalloc");
    exit(EXIT_FAILURE);
  }
  return p;
}

FILE* xfopen(const char* p, const char* m)
{
  FILE* fp = fopen(p, m);
  if (!fp) {
    perror("xfopen");
    exit(EXIT_FAILURE);
  }
  return fp;
}

FILE* xfmemopen(void* b, size_t s, const char* m)
{
  FILE* fp = fmemopen(b, s, m);
  if (!fp) {
    perror("xfmemopen");
    exit(EXIT_FAILURE);
  }
  return fp;
}

char* xstrdup(const char* s)
{
  char* p = strdup(s);
  if (!p) {
    perror("xstrdup");
    exit(EXIT_FAILURE);
  }
  return p;
}

char* xstrndup(const char* s, size_t l)
{
  char* p = strndup(s, l);
  if (!p) {
    perror("xstrndup");
    exit(EXIT_FAILURE);
  }
  return p;
}

/*
 * As I may remember, memdup isn't a part of
 * Glibc/Musl standard C library. But here it is.
 */

#if HAS_MEMDUP == 1
void* xmemdup(const void* m, size_t b)
{
  void* p = memdup(m, b);
  if (!p) {
    perror("xmemdup");
    exit(EXIT_FAILURE);
  }
  return p;
}
#endif

DIR* xopendir(const char* p)
{
  DIR* dir = opendir(p);
  if (!dir) {
    perror("xopendir");
    exit(EXIT_FAILURE);
  }
  return dir;
}