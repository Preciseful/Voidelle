#pragma once

#include <stdio.h>
#include <stdint.h>

#ifdef DEBUG_RELEASE
#define debug(msg, ...) printf(msg, __VA_ARGS__)
#else
#define debug(msg, ...)
#endif

extern FILE *disk;

void init();
void ls(char *path);
void make(char *path, uint64_t flags);