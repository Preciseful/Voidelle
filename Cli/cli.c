#include <string.h>
#include <stdlib.h>

#include "cli.h"

bool InitFilesystem(Voidom *voidom, struct cli_context cli_ctx)
{
    if (create_voidlet(voidom) != SUCCESS)
        return false;

    Voidelle root;
    if (cli_ctx.uid)
        fprintf(stderr, "UID specified: %lu\n", cli_ctx.uid);
    if (cli_ctx.gid)
        fprintf(stderr, "GID specified: %lu\n", cli_ctx.gid);

    if (create_voidelle(*voidom, &root, "/", VOIDELLE_DIRECTORY | VOIDELLE_SYSTEM, cli_ctx.uid, PERMISSION_READ, PERMISSION_READ) != SUCCESS)
        return false;

    return true;
}

bool ValidateFilesystem(Voidom *voidom)
{
    Voidlet voidlet;
    fseek(voidom->disk, 0, SEEK_SET);
    fread(&voidlet, sizeof(voidlet), 1, voidom->disk);

    if (memcmp(voidlet.header, "VOID", 4) != 0)
    {
        printf("Invalid voidlet header.\n");
        return false;
    }

    voidom->voidlet = voidlet;

    Voidelle root;

    fseek(voidom->disk, VOID_SIZE, SEEK_SET);
    fread(&root, sizeof(root), 1, voidom->disk);

    if (memcmp(root.header, "VELLE", 5) != 0)
    {
        printf("Invalid root header.\n");
        return false;
    }

    voidom->root = root;
    return true;
}

const char *GetFilename(const char *path)
{
    const char *last = strrchr(path, '/');
    if (last)
        return last + 1;
    else
        return path;
}

Voidom GetVoidom()
{
    return ((struct cli_context *)fuse_get_context()->private_data)->voidom;
}

bool FindParentVoidelleByPath(Voidom voidom, const char *path, Voidelle *buf)
{
    char *tmp = malloc(strlen(path) + 1);
    strcpy(tmp, path);

    char *slash = strrchr(tmp, '/');
    if (slash == tmp)
        slash[1] = '\0';
    else
        *slash = '\0';

    return FindVoidelleByPath(voidom, tmp, buf);
}

bool FindVoidelleByPath(Voidom voidom, const char *path, Voidelle *buf)
{
    fprintf(stderr, "FINDING VOIDELLE '%s'\n", path);

    if (path[0] != '/')
        return false;
    if (strcmp(path, "/") == 0)
    {
        fprintf(stderr, "ROOT VOIDELLE\n");
        read_void(voidom, &voidom.root, VOID_SIZE, sizeof(Voidelle));
        memcpy(buf, &voidom.root, sizeof(Voidelle));
        return true;
    }

    read_void(voidom, &voidom.root, VOID_SIZE, sizeof(Voidelle));
    Voidelle parent = voidom.root;

    const char *beginning = path + 1;

    while (true)
    {
        const char *end = beginning;
        while (*end != '\0' && *end != '/')
            end++;

        size_t filename_len = end - beginning;

        char *filename = malloc(filename_len + 1);
        memcpy(filename, beginning, filename_len);
        filename[filename_len] = 0;

        uint64_t child_pos = parent.content_voidelle;
        while (child_pos)
        {
            Voidelle child;
            read_void(voidom, &child, child_pos, sizeof(Voidelle));

            char *vname = malloc(child.name_voidelle_size);
            get_voidelle_name(voidom, child, vname);

            fprintf(stderr, "COMPARING FILE '%s' TO FILE '%s'\n", vname, filename);

            if (strcmp(vname, filename) == 0)
            {
                fprintf(stderr, "COMPARATION CORRECT\n");
                free(vname);
                parent = child;
                break;
            }

            fprintf(stderr, "COMPARATION INCORRECT\n");
            free(vname);
            child_pos = child.next_voidelle;
        }

        if (child_pos == 0)
        {
            fprintf(stderr, "NO FOLLOWING CONTENT FOUND\n");
            return false;
        }

        if (*end == '\0')
            break;

        beginning = end + 1;
    }

    if (buf != 0)
        memcpy(buf, &parent, sizeof(Voidelle));
    return true;
}