/*
 * Useful parsing functions.
 * Paul Goldstein, Oct 2024
 */

#pragma once

#include <stdbool.h>
#include "types.h"

int     parse_has_type(const char *line, const char *type);
char   *parse_value(const char *line, const char *type, const char *type_delim);
pair_t *parse_pair(const char *line, const char *type_delim);
vec_t  *parse_vec(const char *line, const char *type, const char *type_delim, const char *val_delim);