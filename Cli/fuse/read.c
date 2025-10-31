#include <errno.h>

#include "../cli.h"

int fuse_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    struct fuse_context *ctx = fuse_get_context();
    Voidom voidom = ((struct cli_context *)ctx->private_data)->voidom;
    Voidelle voidelle;
    if (!read_path(voidom, path, &voidelle, 0))
        return -ENOENT;

    return read_voidelle(voidom, voidelle, offset, buffer, size);
}