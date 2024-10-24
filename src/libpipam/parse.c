#include <stdio.h>
#include <string.h>

#include "log.h"
#include "types.h"
#include "xfunc.h"

int parse_has_type(const char* line, const char* type)
{
  if (!line) {
    pico_log_die(LOG_ERROR, "%s(): line is NULL", __func__);
  }

  if (!type) {
    pico_log_die(LOG_ERROR, "%s(): type is NULL", __func__);
  }

  return strncmp(line, type, strlen(type)) == 0;
}

const char* parse_value(const char* line, const char* type, const char* type_delim)
{
  if (!line) {
    pico_log_die(LOG_ERROR, "%s(): line is NULL", __func__);
  }
  if (!type) {
    pico_log_die(LOG_ERROR, "%s(): type is NULL", __func__);
  }
  if (!type_delim) {
    pico_log_die(LOG_ERROR, "%s(): type_delim is NULL", __func__);
  }

  const char* value   = line + strlen(type) + strlen(type_delim);

  size_t      val_len = strcspn(value, "\n");

  if (val_len == 0)
    pico_log(LOG_WARN, "%s(): parsed value length is 0", __func__);

  return xstrndup(value, val_len);
}

pair_t* parse_pair(const char* line, const char* type_delim)
{
  char *key, *val, *line_val;
  if (!line) {
    pico_log_die(LOG_ERROR, "%s(): type is NULL", __func__);
  }
  if (!type_delim) {
    pico_log_die(LOG_ERROR, "%s(): type is NULL", __func__);
  }

  size_t key_len = strcspn(line, type_delim);

  if (!key_len) {
    pico_log_die(LOG_ERROR, "%s(): pair key length is 0", __func__);
  }

  line_val            = (char*)line + key_len + strlen(type_delim);

  size_t line_val_len = strcspn(val, "\n");

  if (!line_val_len) {
    pico_log_die(LOG_ERROR, "%s(): pair value length is 0", __func__);
  }

  // Is it necessary?

  key         = xstrndup(line, key_len);
  val         = xstrndup(line_val, line_val_len);

  pair_t* res = pair_make(key, val);

  xfree(key);
  xfree(val);

  return res;
}

vec_t* parse_vec(const char* line, const char* type, const char* type_delim, const char* val_delim)
{
  vec_t* vec;
  char * value, *token;

  if (!line) {
    pico_log_die(LOG_ERROR, "%s(): line is NULL", __func__);
  }
  if (!type) {
    pico_log_die(LOG_ERROR, "%s(): type is NULL", __func__);
  }
  if (!type_delim) {
    pico_log_die(LOG_ERROR, "%s(): type_delim is NULL", __func__);
  }

  vec   = vec_init();

  if (!parse_has_type(line, type)) {
    pico_log(LOG_ERROR, "%s(): line doesn't start with type", __func__);
    #if DEBUG == 1
    pico_log(LOG_DEBUG, "%s(): line: %s", __func__, line);
    #endif
    goto end;
  }

  value = (char*)line + strlen(type) + strlen(type_delim);

  token = strtok((char*)value, val_delim);
  while (token != NULL) {
    token[strcspn(token, "\n")] = '\0';
    if (!strlen(token))
      goto next_token;
    vec_push_back(vec, token);
  next_token:
    token = strtok(NULL, val_delim);
  }

#if DEBUG == 1
  pico_log(LOG_DEBUG, "%s(): parsed vector have %lu elements", __func__, pkg->files->len);
#endif

end:
  return vec;
}
