#include "cli.h"
#include <string.h>
#include <stdlib.h>

bool init_filesystem(Voidom *voidom)
{
    if (create_voidlet(voidom) != SUCCESS)
        return false;

    Voidelle root;
    if (create_voidelle(*voidom, &root, "/", VOIDELLE_DIRECTORY | VOIDELLE_SYSTEM, 0x2, 0x2) != SUCCESS)
        return false;

    return true;
}

bool validate_filesystem(Voidom *voidom)
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

bool find_voidelle_by_name(Voidom voidom, char *name, Voidelle parent, Voidelle *buf)
{
    uint64_t pos = parent.content_voidelle;
    while (pos)
    {
        Voidelle voidelle;
        read_void(voidom, &voidelle, pos, sizeof(Voidelle));

        char *v_name = malloc(voidelle.name_voidelle_size);
        get_voidelle_name(voidom, voidelle, v_name);
        fprintf(stderr, "Searching: %s\n", v_name);

        if (strcmp(v_name, name) == 0)
        {
            free(v_name);
            memcpy(buf, &voidelle, sizeof(Voidelle));
            return true;
        }

        free(v_name);
        pos = voidelle.next_voidelle;
    }

    return false;
}

bool read_path(Voidom voidom, const char *path, Voidelle *voidelle, size_t offset)
{
    const char *p = path;
    size_t paths_count = 1;
    char **paths = malloc(sizeof(char **));
    paths[0] = malloc(2);
    memcpy(paths[0], "/", 2);

    while (*p)
    {
        while (*p == '/')
            p++;

        if (!*p)
            break;

        const char *start = p;
        while (*p && *p != '/')
            p++;

        size_t len = p - start;

        paths = realloc(paths, sizeof(char **) * (paths_count + 1));
        paths[paths_count] = malloc(len + 1);
        memcpy(paths[paths_count], start, len);
        paths[paths_count][len] = 0;

        paths_count++;
    }

    read_void(voidom, &voidom.root, voidom.root.position, sizeof(Voidelle));

    Voidelle parent = voidom.root;
    if (offset > paths_count)
        return false;

    paths_count -= offset;

    for (unsigned long i = 1; i < paths_count; i++)
    {
        if (!find_voidelle_by_name(voidom, paths[i], parent, &parent))
            return false;
    }

    memcpy(voidelle, &parent, sizeof(Voidelle));
    return true;
}