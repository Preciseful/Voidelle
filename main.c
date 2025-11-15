#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#include "Filesystem/voidelle.h"
#include "Cli/cli.h"

struct cli_context cli_ctx = {
    .disk = 0,
    .init = false,
    .uid = 0,
    .gid = 0,
};

static struct fuse_operations operations = {
    .getattr = FuseGetAttributes,
    .readdir = FuseReadDirectory,
    .read = FuseReadFile,
    .create = FuseCreateFile,
};

static const struct fuse_opt option_spec[] = {
    {"--disk=%s", offsetof(struct cli_context, disk), 0},
    {"--init", offsetof(struct cli_context, init), 1},
    {"--user=%lu", offsetof(struct cli_context, uid), 0},
    {"--gid=%lu", offsetof(struct cli_context, gid), 0},
    FUSE_OPT_END,
};

int main(int argc, char *argv[])
{
    // disk's always there
    int custom_arg_count = 1;
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    fuse_opt_parse(&args, &cli_ctx, option_spec, NULL);

    if (cli_ctx.uid)
        custom_arg_count++;

    if (cli_ctx.gid)
        custom_arg_count++;
    else
        cli_ctx.gid = cli_ctx.uid;

    if (!cli_ctx.disk)
    {
        printf("Expected disk argument.\n");
        return 1;
    }

    cli_ctx.voidom.disk = fopen(cli_ctx.disk, "r+");
    printf("Disk: %s\n", cli_ctx.disk);
    if (!cli_ctx.voidom.disk)
    {
        printf("Invalid disk.\n");
        return 1;
    }

    if (cli_ctx.init)
    {
        printf("Initializing filesystem.\n");
        if (!InitFilesystem(&cli_ctx.voidom, cli_ctx))
            printf("Error encountered.\n");

        return 1;
    }

    else if (!ValidateFilesystem(&cli_ctx.voidom))
    {
        fclose(cli_ctx.voidom.disk);
        printf("Invalid filesystem.\n");
        return 1;
    }

    return fuse_main(argc - custom_arg_count, argv, &operations, &cli_ctx);
}