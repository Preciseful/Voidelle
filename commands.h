#pragma once

#include <stdio.h>
#include <stdint.h>

#ifdef DEBUG_RELEASE
#define debug(msg, ...) printf(msg, __VA_ARGS__)
#else
#define debug(msg, ...)
#endif

extern FILE *disk;

enum Ls_Options
{
    LS_NONE,
    LS_TREE,
    LS_LONG
};

void init();
void ls(char *path, enum Ls_Options flag);
void make(char *path, uint64_t flags);