#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "voidelle.h"
#include "commands.h"

char *usage = "Usage: voidelle DISK [OPTION]... PATH...\n"
              "Interact with a Voidelle filesystem.\n"
              "\n"
              "A DISK argument is required in order to interact with the filesystem.\n"
              "PATH must be absolute.\n"
              "\n"
              "Commands:\n"
              " init    initializes the disk with the Voidelle filesystem\n"
              "          THIS MAKES PREVIOUS DATA UNUSABLE\n"
              " ls      list the entries in PATH\n"
              " tree    list the entries in PATH in a tree format\n"
              " touch   creates the files in PATH\n"
              " mkdir   creates the directories in PATH\n"
              " rm      removes the entries in PATH\n"
              " wr      write to a file\n"
              " cat     display PATH file's content\n";

char *ls_usage = "Usage: ls [OPTION...] PATH...\n"
                 "List the entries in PATH.\n"
                 "\n"
                 "Options:\n"
                 " -l   display extra data about the entries\n";

char *rm_usage = "Usage: rm [OPTION...] PATH...\n"
                 "Remove the entries in PATH.\n"
                 "\n"
                 "Options:\n"
                 " -r   remove content recursively (used for directories)\n";

char *mkdir_usage = "Usage: mkdir [OPTION...] PATH...\n"
                    "Create the directories in PATH.\n"
                    "\n"
                    "Options:\n"
                    " -r   create directories recursively\n";

char *wr_usage = "Usage: wr PATH DATA\n"
                 "Write DATA in the entry found in PATH.\n";

char *eat_arg(int *argc, char **argv[])
{
    if (*argc == 0)
    {
        printf("Expected argument.\n");
        exit(-1);
    }

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
        init();

    // check for VOID before going further
    else if (memcmp(get_voidlet().identifier, "VOID", 4) != 0)
        printf("Invalid voidelle filesystem.\n");

    else if (strcmp(option, "tree") == 0)
    {
        if (argc == 0)
            ls("", LS_TREE);
        while (argc)
            ls(eat_arg(&argc, &argv), LS_TREE);
    }

    else if (strcmp(option, "ls") == 0)
    {
        if (argc == 0 ||
            (argc != 0 && (strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0)))
        {
            eat_arg(&argc, &argv);
            puts(ls_usage);
            goto finish;
        }

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
            make(eat_arg(&argc, &argv), 0, false);
    }

    else if (strcmp(option, "mkdir") == 0)
    {
        if (argc == 0 ||
            (argc != 0 && (strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0)))
        {
            eat_arg(&argc, &argv);
            puts(mkdir_usage);
            goto finish;
        }

        bool recursive = false;
        if (argc != 0 && strcmp(*argv, "-r") == 0)
        {
            recursive = true;
            eat_arg(&argc, &argv);
        }

        while (argc)
            make(eat_arg(&argc, &argv), VOIDELLE_DIRECTORY, recursive);
    }

    else if (strcmp(option, "rm") == 0)
    {
        if (argc == 0 ||
            (argc != 0 && (strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0)))
        {
            eat_arg(&argc, &argv);
            puts(rm_usage);
            goto finish;
        }

        bool recursive = false;
        if (argc != 0 && strcmp(*argv, "-r") == 0)
        {
            recursive = true;
            eat_arg(&argc, &argv);
        }

        while (argc)
            rm_file(eat_arg(&argc, &argv), recursive);
    }

    else if (strcmp(option, "wr") == 0)
    {
        if (argc == 0 ||
            (argc != 0 && (strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0)))
        {
            eat_arg(&argc, &argv);
            puts(wr_usage);
            goto finish;
        }

        char *path = eat_arg(&argc, &argv);
        char *data = eat_arg(&argc, &argv);
        write(path, data);
    }

    else if (strcmp(option, "cat") == 0)
    {
        while (argc)
            cat(eat_arg(&argc, &argv));
    }

    else
        printf("Unknown option: '%s'.\n", option);

finish:
    if (argc != 0)
        printf("No further arguments can be taken.\n");
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        puts(usage);
        return 0;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
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