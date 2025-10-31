#pragma once

#define FUSE_USE_VERSION 30

#include <fuse3/fuse.h>
#include <stdbool.h>
#include "../Filesystem/voidelle.h"

#define PERM(owner, others)                                                                                                              \
    (((owner & PERMISSION_READ ? S_IRUSR : 0) | (owner & PERMISSION_WRITE ? S_IWUSR : 0) | (owner & PERMISSION_EXECUTE ? S_IXUSR : 0)) | \
     ((others & PERMISSION_READ ? S_IROTH : 0) | (others & PERMISSION_WRITE ? S_IWOTH : 0) | (others & PERMISSION_EXECUTE ? S_IXOTH : 0)))

#define OWNER_BITS(mode)                         \
    (((mode & S_IRUSR) ? PERMISSION_READ : 0) |  \
     ((mode & S_IWUSR) ? PERMISSION_WRITE : 0) | \
     ((mode & S_IXUSR) ? PERMISSION_EXECUTE : 0))

#define OTHER_BITS(mode)                         \
    (((mode & S_IROTH) ? PERMISSION_READ : 0) |  \
     ((mode & S_IWOTH) ? PERMISSION_WRITE : 0) | \
     ((mode & S_IXOTH) ? PERMISSION_EXECUTE : 0))

struct cli_context
{
    const char *disk;
    bool init;
    int uid;
    Voidom voidom;
};

bool init_filesystem(Voidom *voidom);
bool validate_filesystem(Voidom *voidom);
bool read_path(Voidom voidom, const char *path, Voidelle *voidelle, size_t offset);
int getattr(struct cli_context *cli_ctx, Voidelle voidelle, struct stat *st);
int create_fuse_voidelle(const char *path, mode_t mode, enum Voidelle_Flags flags);

int fuse_getattr(const char *path, struct stat *st, struct fuse_file_info *fi);
int fuse_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi);
int fuse_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags);
int fuse_touch(const char *path, mode_t mode, dev_t dev);
int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi);
int fuse_update_time(const char *path, const struct timespec *tv, struct fuse_file_info *fi);
int fuse_mkdir(const char *path, mode_t mode);
int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);