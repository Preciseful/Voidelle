#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "../cli.h"

int GetAttributes(struct cli_context *cli_ctx, Voidelle voidelle, struct stat *st)
{
    if (cli_ctx->gid)
        fprintf(stderr, "GID specified: %lu\n", cli_ctx->gid);

    fprintf(stderr, "GET ATTRIBUTES: %lu > %lu\n", voidelle.position, voidelle.next_voidelle);
    st->st_uid = voidelle.owner_id;
    st->st_gid = cli_ctx->gid ? cli_ctx->gid : getgid();
    st->st_atime = voidelle.access_seconds;
    st->st_mtime = voidelle.modification_seconds;
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

int FuseGetAttributes(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    struct cli_context *cli_ctx = fuse_get_context()->private_data;

    Voidelle voidelle;
    if (!FindVoidelleByPath(cli_ctx->voidom, path, &voidelle))
        return -ENOENT;

    return GetAttributes(cli_ctx, voidelle, st);
}