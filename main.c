#include <stdio.h>
#include <argp.h>
#include <stdlib.h>
#include <Cli/cli.h>
#include <string.h>

struct arguments
{
    disk_t disk;
    char *command;
    char **commands;
};

static int parse_arg(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
    case 'd':
        arguments->disk = fopen(arg, "r+");
        break;

    case ARGP_KEY_ARG:
        if (!arguments->command)
            arguments->command = arg;
        else
        {
            arguments->commands = &state->argv[state->next - 1];
            state->next = state->argc;
        }
        break;

    case ARGP_KEY_END:
        if (!arguments->disk)
        {
            argp_failure(state, 1, 0, "required -d (--disk). See --help for more information");
            exit(ARGP_ERR_UNKNOWN);
        }

    default:
        break;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct arguments arguments;
    arguments.disk = 0;

    struct argp_option options[] =
        {
            {"disk", 'd', "DISK", 0, "Choose disk"},
            {0},
        };

    struct argp argp = {options, parse_arg};
    error_t error = argp_parse(&argp, argc, argv, 0, 0, &arguments);
    if (error)
        return error;

    if (strcmp(arguments.command, "init") == 0)
        init_filesystem(arguments.disk);

    return 0;
}