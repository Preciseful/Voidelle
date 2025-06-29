#include <stdio.h>
#include <string.h>
#include "voidelle.h"
#include "commands.h"

char *usage = "Usage: voidelle <disk> [OPTION]... [FILES]...\n"
              "Interact with a Voidelle filesystem.\n";

char *eat_arg(int *argc, char **argv[])
{
    char *ret = **argv;
    (*argv)++;
    (*argc)--;
    return ret;
}

void execute(int argc, char *argv[])
{
    char *disk_name = eat_arg(&argc, &argv);
    char *option = eat_arg(&argc, &argv);

    debug("Using disk: '%s'\n", disk_name);
    debug("Option: %s (out of %d)\n", option, argc);

    disk = fopen(disk_name, "r+");
    if (disk == 0)
    {
        printf("Failed opening disk %s.\n", disk_name);
        return;
    }

    if (memcmp(get_voidlet().identifier, "VOID", 4) != 0)
    {
        printf("Invalid voidelle filesystem.\n");
        return;
    }

    if (strcmp(option, "init") == 0)
    {
        init();
        if (argc != 0)
            printf("No further arguments can be taken.\n");
    }

    else if (strcmp(option, "tree") == 0)
    {
        if (argc == 0)
            ls("", LS_TREE);
        while (argc)
            ls(eat_arg(&argc, &argv), LS_TREE);
    }

    else if (strcmp(option, "ls") == 0)
    {
        enum Ls_Options option = LS_NONE;
        if (argc != 0 && strcmp(*argv, "-l") == 0)
        {
            option = LS_LONG;
            eat_arg(&argc, &argv);
        }

        if (argc == 0)
            ls("", option);

        while (argc)
        {
            ls(eat_arg(&argc, &argv), option);
            if (argc > 0)
                printf("\n");
        }
    }

    else if (strcmp(option, "touch") == 0)
    {
        while (argc)
            make(eat_arg(&argc, &argv), 0);
    }

    else if (strcmp(option, "mkdir") == 0)
    {
        while (argc)
            make(eat_arg(&argc, &argv), VOIDELLE_DIRECTORY);
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        puts(usage);
        return 0;
    }

    if (argc == 2)
    {
        puts("Expected option argument.");
        return 0;
    }

    execute(argc - 1, argv + 1);
    if (disk != 0)
        fclose(disk);
}