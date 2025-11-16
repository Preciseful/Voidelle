#include <errno.h>

#include "../cli.h"

int FuseRemoveEntry(const char *path)
{
    struct fuse_context *ctx = fuse_get_context();
    Voidom voidom = ((struct cli_context *)ctx->private_data)->voidom;
    Voidelle voidelle;
    Voidelle parent;

    if (!FindVoidelleByPath(voidom, path, &voidelle))
        return -ENOENT;
    FindParentVoidelleByPath(voidom, path, &parent);

    if (remove_voidelle(voidom, &parent, voidelle, true))
    {
        clear_voidelle_content(voidom, &voidelle);
        clear_voidelle_name(voidom, &voidelle);
        return 0;
    }

    return -EIO;
}