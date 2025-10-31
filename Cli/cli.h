#pragma once

#define FUSE_USE_VERSION 30

#include <fuse3/fuse.h>
#include <stdbool.h>
#include "../Filesystem/voidelle.h"

struct cli_context
{
    const char *disk;
    bool init;
    unsigned long uid;
    Voidom voidom;
};

bool init_filesystem(Voidom *voidom);
bool validate_filesystem(Voidom *voidom);
bool read_path(Voidom voidom, const char *path, Voidelle *voidelle, size_t offset);
int getattr(struct cli_context *cli_ctx, Voidelle voidelle, struct stat *st);

int fuse_getattr(const char *path, struct stat *st, struct fuse_file_info *fi);
int fuse_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi);
int fuse_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags);