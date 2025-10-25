#include <stdio.h>
#include <argp.h>
#include <stdlib.h>

struct arguments
{
    char *disk;
    char **rest;
};

static int parse_arg(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
    case 'd':
        arguments->disk = arg;
        break;

    case ARGP_KEY_ARG:
        arguments->rest = &state->argv[state->next - 1];
        state->next = state->argc;
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

    return 0;
}