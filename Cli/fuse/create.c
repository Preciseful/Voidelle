#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../cli.h"

int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    return fuse_touch(path, mode, 0);
}