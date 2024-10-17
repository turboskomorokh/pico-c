/*
 * Some cool important config values libpipam/pico REALLY depends on
 * Paul Goldstein, Oct 2024
 */

#pragma once

// DANGEROUS
#define PICO_MAX_REMLEVEL 2

#ifndef PICO_VAR_PATH
#define PICO_VAR_PATH PICO_PREFIX "/" "var/lib/pico"
#endif // PICO_VAR_PATH

#ifndef PICO_DB_PATH
#define PICO_DB_PATH PICO_PREFIX "/" PICO_VAR_PATH "/db"
#endif // PICO_DB_PATH

#ifndef PICO_PREFIX
#define PICO_PREFIX "/"
#endif // PICO_PREFIX

#ifndef PICO_ROOT_CHECK
#define PICO_ROOT_CHECK 1
#endif // PICO_ROOT_CHECK

