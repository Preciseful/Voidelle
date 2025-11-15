#include "../cli.h"

int fuse_mkdir(const char *path, mode_t mode)
{
    return create_fuse_voidelle(path, mode, VOIDELLE_DIRECTORY, 0);
}
