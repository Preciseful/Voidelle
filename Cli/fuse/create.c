#include <errno.h>
#include <stdlib.h>

#include "../cli.h"

int CreateEntry(const char *path, mode_t mode, bool dir)
{
    struct cli_context *cli_ctx = fuse_get_context()->private_data;
    Voidom voidom = GetVoidom();

    if (FindVoidelleByPath(voidom, path, 0))
        return -EEXIST;

    enum Voidelle_Flags dir_flag = dir ? VOIDELLE_DIRECTORY : 0;
    uint64_t uid = cli_ctx->uid;
    uint8_t owner_perm = OWNER_BITS(mode);
    uint8_t other_perm = OTHER_BITS(mode);

    fprintf(stderr, "CREATING FILE: '%s'\n", GetFilename(path));

    Voidelle voidelle;
    create_voidelle(voidom, &voidelle, GetFilename(path), dir_flag, uid, owner_perm, other_perm);

    Voidelle parent;
    FindParentVoidelleByPath(voidom, path, &parent);

    add_voidelle(voidom, &parent, voidelle);

    return 0;
}

int FuseCreateFile(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    return CreateEntry(path, mode, false);
}

int FuseCreateDirectory(const char *path, mode_t mode)
{
    return CreateEntry(path, mode, true);
}
