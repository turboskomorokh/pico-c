/*
 * Leak-safe memory functions. Used across the project.
 * Paul Goldstein, Oct 2024
 */

#pragma once

#include <dirent.h>
#include <stddef.h>
#include <stdio.h>

// Safe memory functions
void  xfree(void* ptr);

void* xmalloc(size_t s);
void* xrealloc(void* p, size_t s);
void* xcalloc(size_t nmemb, size_t s);

// Safe file functions
FILE* xfopen(const char* p, const char* m);
FILE* xfmemopen(void* b, size_t s, const char* m);

// Safe string functions
char* xstrdup(const char* s);
char* xstrndup(const char* s, size_t l);

// Safe folder functions
DIR *xopendir(const char *p);
