/*
 * Some sad lonely definitions and functions.
 * I love them as my own.
 * Paul Goldstein, Oct 2024
 */
#pragma once

#define EXIT_ERROR 2

int fexists(const char *path);
int flexists(const char *path);
int fisreg(const char *path);
int fisdir(const char *path);
int fislnk(const char *path);

int is_root();