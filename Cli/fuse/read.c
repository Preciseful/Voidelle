#include <errno.h>

#include "../cli.h"

int FuseReadFile(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    Voidom voidom = GetVoidom();

    Voidelle voidelle;
    if (!FindVoidelleByPath(voidom, path, &voidelle))
        return -ENOENT;

    return read_voidelle(voidom, voidelle, offset, buffer, size);
}