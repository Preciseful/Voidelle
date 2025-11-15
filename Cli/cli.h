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
    int gid;
    Voidom voidom;
};

bool InitFilesystem(Voidom *voidom, struct cli_context cli_ctx);
bool ValidateFilesystem(Voidom *voidom);
const char *GetFilename(const char *path);
Voidom GetVoidom();
bool FindParentVoidelleByPath(Voidom voidom, const char *path, Voidelle *buf);
bool FindVoidelleByPath(Voidom voidom, const char *path, Voidelle *buf);

int GetAttributes(struct cli_context *cli_ctx, Voidelle voidelle, struct stat *st);

int FuseReadDirectory(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags);
int FuseGetAttributes(const char *path, struct stat *st, struct fuse_file_info *fi);
int FuseReadFile(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi);
int FuseCreateFile(const char *path, mode_t mode, struct fuse_file_info *fi);
int FuseCreateDirectory(const char *path, mode_t mode);
int FuseUpdateTime(const char *path, const struct timespec *tv, struct fuse_file_info *fi);