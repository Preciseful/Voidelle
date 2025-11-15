#include <errno.h>

#include "../cli.h"

int fuse_remove(const char *path)
{
    struct fuse_context *ctx = fuse_get_context();
    Voidom voidom = ((struct cli_context *)ctx->private_data)->voidom;
    Voidelle voidelle;
    Voidelle parent;

    if (!read_path(voidom, path, &voidelle, 0))
        return -ENOENT;
    read_path(voidom, path, &parent, 1);

    if (remove_voidelle(voidom, &parent, voidelle, 0x3))
        return 0;
    return -EIO;
}