#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <utime.h>

#include "../cli.h"

// we define this as i cant access it for wtv reason
#define UTIME_NOW ((1l << 30) - 1l)
#define UTIME_OMIT ((1l << 30) - 2l)

int fuse_update_time(const char *path, const struct timespec *tv, struct fuse_file_info *fi)
{
    struct cli_context *ctx = fuse_get_context()->private_data;
    Voidelle voidelle;
    read_path(ctx->voidom, path, &voidelle, 0);
    fprintf(stderr, "UPDATE TIME TO: %s\n", path);

    time_t now = time(0);

    if (tv[0].tv_nsec != UTIME_OMIT)
        voidelle.access_seconds = (tv[0].tv_nsec == UTIME_NOW) ? now : tv[0].tv_sec;
    if (tv[1].tv_nsec != UTIME_OMIT)
        voidelle.modification_seconds = (tv[1].tv_nsec == UTIME_NOW) ? now : tv[1].tv_sec;

    write_void(ctx->voidom, &voidelle, voidelle.position, sizeof(Voidelle));

    return 0;
}