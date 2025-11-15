#include <errno.h>
#include <stdlib.h>

#include "../cli.h"

int FuseReadDirectory(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    struct stat st;
    struct fuse_context *ctx = fuse_get_context();
    Voidom voidom = GetVoidom();

    Voidelle voidelle;
    if (!FindVoidelleByPath(voidom, path, &voidelle))
        return -ENOENT;

    uint64_t pos = voidelle.content_voidelle;

    while (pos)
    {
        read_void(voidom, &voidelle, pos, sizeof(Voidelle));
        GetAttributes(ctx->private_data, voidelle, &st);

        char *name = malloc(voidelle.name_voidelle_size);
        get_voidelle_name(voidom, voidelle, name);

        filler(buffer, name, &st, 0, 0);

        free(name);
        pos = voidelle.next_voidelle;
    }

    return 0;
}