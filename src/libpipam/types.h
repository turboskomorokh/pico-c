/*
 * Vector is OP.
 * It's kinda funny, pair is never used.
 * Paul Goldstein, Oct 2024
 */
#pragma once

#include <stddef.h>

typedef struct vector vec_t;
typedef struct pair   pair_t;

#define VEC_INIT_CAP 4

struct vector {
  char** data;
  size_t cap;
  size_t len;
};

vec_t* vec_init();
void   vec_free(vec_t *v);
void   vec_push_back(vec_t* v, const char* str);
const char* vec_get(vec_t* v, size_t index);
char*  vес_pop_back(vec_t* v);
void   vес_remove(vec_t* v, size_t index);

struct pair {
  char* key;
  char* value;
};

pair_t* pair_make(char* key, char* value);
void    pair_free(pair_t* pair);