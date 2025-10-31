#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "../cli.h"

int getattr(struct cli_context *cli_ctx, Voidelle voidelle, struct stat *st)
{
    struct tm mt = {0};
    mt.tm_year = voidelle.modification_year - 1900;
    mt.tm_mon = voidelle.modification_date[0] - 1;
    mt.tm_mday = voidelle.modification_date[1];
    mt.tm_hour = voidelle.modification_date[2];
    mt.tm_min = voidelle.modification_date[3];
    mt.tm_sec = voidelle.modification_date[4];

    struct tm ct = {0};
    ct.tm_year = voidelle.creation_year - 1900;
    ct.tm_mon = voidelle.creation_date[0];
    ct.tm_mday = voidelle.creation_date[1];
    ct.tm_hour = voidelle.creation_date[2];
    ct.tm_min = voidelle.creation_date[3];
    ct.tm_sec = voidelle.creation_date[4];

    if (cli_ctx->uid)
        fprintf(stderr, "UID specified: %lu\n", cli_ctx->uid);

    st->st_uid = cli_ctx->uid ? cli_ctx->uid : voidelle.owner_id;
    st->st_gid = getgid();
    st->st_atime = mktime(&mt);
    st->st_mtime = mktime(&mt);
    st->st_ctime = mktime(&ct);
    st->st_size = voidelle.content_voidelle_size;

    char *name = malloc(voidelle.name_voidelle_size);
    get_voidelle_name(cli_ctx->voidom, voidelle, name);

    if (voidelle.flags & VOIDELLE_DIRECTORY)
    {
        st->st_mode = __S_IFDIR | 0755;
        st->st_nlink = 2;
    }
    else
    {
        st->st_mode = __S_IFREG | 0644;
        st->st_nlink = 1;
    }

    free(name);
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