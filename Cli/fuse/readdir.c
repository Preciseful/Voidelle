#include <errno.h>
#include <stdlib.h>

#include "../cli.h"

int fuse_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    struct stat st;
    struct fuse_context *ctx = fuse_get_context();
    Voidom voidom = ((struct cli_context *)ctx->private_data)->voidom;

    Voidelle voidelle;
    if (!read_path(voidom, path, &voidelle, 0))
        return -ENOENT;

    uint64_t pos = voidelle.content_voidelle;

    while (pos)
    {
        read_void(voidom, &voidelle, pos, sizeof(Voidelle));
        getattr(ctx->private_data, voidelle, &st);

        char *name = malloc(voidelle.name_voidelle_size);
        get_voidelle_name(voidom, voidelle, name);

        filler(buffer, name, &st, 0, 0);

        free(name);
        pos = voidelle.next_voidelle;
    }

    return 0;
}