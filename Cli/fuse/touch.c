#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../cli.h"

int create_fuse_voidelle(const char *path, mode_t mode, enum Voidelle_Flags flags)
{
    struct fuse_context *fuse_ctx = fuse_get_context();
    struct cli_context *cli_ctx = fuse_ctx->private_data;

    Voidelle parent;
    Voidelle voidelle;

    if (!read_path(cli_ctx->voidom, path, &parent, 1))
        return -ENOENT;
    if (read_path(cli_ctx->voidom, path, &voidelle, 0))
        return -EEXIST;

    unsigned long path_len = strlen(path);
    unsigned long name_index = 0;

    for (unsigned long i = path_len - 1; i >= 0; i--)
    {
        if (path[i] != '/')
            break;
        path_len--;
    }

    for (unsigned long i = path_len - 1; i >= 0; i--)
    {
        if (path[i] != '/')
            continue;

        name_index = i + 1;
        break;
    }

    unsigned long name_len = path_len - name_index + 1;
    char *name = malloc(name_len);
    memcpy(name, path + name_index, name_len);
    name[name_len - 1] = 0;

    uint8_t owner_bits = OWNER_BITS(mode);
    uint8_t other_bits = OTHER_BITS(mode);

    create_voidelle(cli_ctx->voidom, &voidelle, name, flags, owner_bits, other_bits);

    if (!parent.content_voidelle)
    {
        parent.content_voidelle = voidelle.position;
        write_void(cli_ctx->voidom, &parent, parent.position, sizeof(parent));
    }
    else
    {
        Voidelle neighbour;
        unsigned long pos = parent.content_voidelle;

        while (pos)
        {
            Voidelle neighbour;
            read_void(cli_ctx->voidom, &neighbour, pos, sizeof(neighbour));

            pos = neighbour.next_voidelle;
        }

        neighbour.next_voidelle = voidelle.position;
        write_void(cli_ctx->voidom, &neighbour, neighbour.position, sizeof(neighbour));
    }

    return 0;
}

int fuse_touch(const char *path, mode_t mode, dev_t dev)
{
    return create_fuse_voidelle(path, mode, 0);
}
