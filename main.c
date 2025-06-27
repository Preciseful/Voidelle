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

    if (strcmp(option, "init") == 0)
    {
        init();
        if (argc != 0)
            printf("No further arguments can be taken.\n");
    }

    else if (strcmp(option, "ls") == 0)
    {
        for (int i = 0; i < argc; i++)
            ls(eat_arg(&argc, &argv));
    }

    else if (strcmp(option, "touch") == 0)
    {
        for (int i = 0; i < argc; i++)
            make(eat_arg(&argc, &argv), 0);
    }

    else if (strcmp(option, "mkdir") == 0)
    {
        for (int i = 0; i < argc; i++)
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
    fclose(disk);
}