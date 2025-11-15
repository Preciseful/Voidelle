#include <errno.h>
#include <stdlib.h>

#include "../cli.h"

int FuseCreateFile(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    struct cli_context *cli_ctx = fuse_get_context()->private_data;
    Voidom voidom = GetVoidom();

    if (FindVoidelleByPath(voidom, path, 0))
        return -EEXIST;

    enum Voidelle_Flags is_dir = ((mode & __S_IFMT) == __S_IFDIR) ? VOIDELLE_DIRECTORY : 0;
    uint64_t uid = cli_ctx->uid;
    uint8_t owner_perm = OWNER_BITS(mode);
    uint8_t other_perm = OTHER_BITS(mode);

    fprintf(stderr, "CREATING FILE: '%s'\n", GetFilename(path));

    Voidelle voidelle;
    create_voidelle(voidom, &voidelle, GetFilename(path), is_dir, uid, owner_perm, other_perm);

    Voidelle parent;
    FindParentVoidelleByPath(voidom, path, &parent);

    add_voidelle(voidom, &parent, voidelle);

    return 0;
}