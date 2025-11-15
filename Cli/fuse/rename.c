#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <linux/fs.h>

#include "../cli.h"

int rename_exchange(Voidom voidom, Voidelle source, const char *source_path, const char *destination_path)
{
    fprintf(stderr, "RENAME: EXCHANGE\n");

    Voidelle destination;
    if (!read_path(voidom, destination_path, &destination, 0))
        return -ENOENT;

    swap_voidelles(voidom, &source, &destination, 0x1);
    return 0;
}

int rename_noreplace(Voidom voidom, Voidelle source, const char *source_path, const char *destination_path)
{
    fprintf(stderr, "RENAME: NO REPLACE\n");
    Voidelle destination;

    int error = create_fuse_voidelle(destination_path, PERM(source.owner_permission, source.other_permission), source.flags, &destination);
    if (error)
        return error;

    swap_voidelles(voidom, &source, &destination, 0x2);

    Voidelle source_parent;
    read_path(voidom, source_path, &source_parent, 1);

    remove_voidelle(voidom, &source_parent, destination, 0x3);

    return 0;
}

int rename_normal(Voidom voidom, Voidelle source, const char *source_path, const char *destination_path)
{
    fprintf(stderr, "RENAME: NORMAL\n");

    Voidelle destination;
    if (!read_path(voidom, destination_path, &destination, 0))
        return rename_noreplace(voidom, source, source_path, destination_path);

    rename_exchange(voidom, source, source_path, destination_path);

    Voidelle source_parent;
    read_path(voidom, source_path, &source_parent, 1);
    remove_voidelle(voidom, &source_parent, destination, 0x3);

    return 0;
}

int fuse_rename(const char *source_path, const char *destination_path, unsigned int flags)
{
    struct fuse_context *fuse_ctx = fuse_get_context();
    struct cli_context *cli_ctx = fuse_ctx->private_data;

    Voidelle voidelle;
    if (!read_path(cli_ctx->voidom, source_path, &voidelle, 0))
        return -ENOENT;

    if (flags == RENAME_EXCHANGE)
        return rename_exchange(cli_ctx->voidom, voidelle, source_path, destination_path);
    else if (flags == RENAME_NOREPLACE)
        return rename_noreplace(cli_ctx->voidom, voidelle, source_path, destination_path);
    else
        return rename_normal(cli_ctx->voidom, voidelle, source_path, destination_path);
}