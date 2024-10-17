#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "xfunc.h"

vec_t* vec_init()
{
  vec_t* v = xmalloc(sizeof(vec_t));
  v->data  = (char**)xmalloc(VEC_INIT_CAP * sizeof(char*));
  v->cap   = VEC_INIT_CAP;
  v->len   = 0;
  return v;
};

void vec_push_back(vec_t* v, const char* str)
{
  if (v->len == v->cap) {
    v->cap *= 2;
    v->data = (char**)xrealloc(v->data, v->cap * sizeof(char*));
  }

  v->data[v->len] = xstrdup(str);
  v->len++;
  return;
}

const char* vec_get(vec_t* v, size_t index)
{
  if (index >= v->len) {
    fprintf(stderr, __FILE__ ":vec_get(): index out of bounds\n");
    return NULL;
  }
  return v->data[index];
}

void vес_remove(vec_t* v, size_t index)
{
  if (index >= v->len) {
    fprintf(stderr, __FILE__ ":vес_remove(): index out of bounds\n");
    return;
  }

  xfree(v->data[index]);

  for (size_t i = index; i < v->len - 1; i++) {
    v->data[i] = v->data[i + 1];
  }

  v->len--;
  return;
}

char* vес_pop_back(vec_t* v)
{
  if (v->len == 0) {
    return NULL;
  }
  char* last = v->data[v->len - 1];

  v->len--;

  return last;
}

void vec_free(vec_t* v)
{
  if(v) {

  for (size_t i = 0; i < v->len; i++) {
    xfree(v->data[i]);
  }
  xfree(v->data);
  xfree(v);
  return;
  }
}

pair_t* pair_make(char* k, char* v)
{
  pair_t* pair = xmalloc(sizeof(pair_t));
  pair->key    = xstrdup(k);
  pair->value  = xstrdup(v);
  return pair;
}

void pair_free(pair_t* pair)
{
  if (pair) {
    if (pair->key) {
      xfree(pair->key);
    }
    if (pair->value) {
      xfree(pair->value);
    }
    xfree(pair);
  }
  return;
}