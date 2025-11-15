#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "../cli.h"

int getattr(struct cli_context *cli_ctx, Voidelle voidelle, struct stat *st)
{
    if (cli_ctx->uid)
        fprintf(stderr, "UID specified: %lu\n", cli_ctx->uid);

    st->st_uid = cli_ctx->uid ? cli_ctx->uid : voidelle.owner_id;
    st->st_gid = getgid();
    st->st_atime = voidelle.access_seconds;
    st->st_mtime = voidelle.modification_seconds;
    st->st_ctime = voidelle.creation_seconds;
    st->st_size = voidelle.content_voidelle_size;

    mode_t mode = PERM(voidelle.owner_permission, voidelle.other_permission);

    if (voidelle.flags & VOIDELLE_DIRECTORY)
    {
        st->st_mode = __S_IFDIR | mode;
        st->st_nlink = 2;
    }
    else
    {
        st->st_mode = __S_IFREG | mode;
        st->st_nlink = 1;
    }

    return 0;
}

int fuse_getattr(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    struct fuse_context *ctx = fuse_get_context();
    struct cli_context *cli_ctx = ctx->private_data;
    Voidelle voidelle;

    if (!read_path(cli_ctx->voidom, path, &voidelle, 0))
        return -ENOENT;

    return getattr(cli_ctx, voidelle, st);
}