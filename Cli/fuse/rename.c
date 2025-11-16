#include <errno.h>
#include <linux/fs.h>
#include <string.h>

#include "../cli.h"

int rename_exchange(const char *source_path, const char *destination_path)
{
    fprintf(stderr, "RENAME EXCHANGE BETWEEN '%s' AND '%s'\n", source_path, destination_path);
    Voidom voidom = GetVoidom();

    Voidelle source, destination;
    if (!FindVoidelleByPath(voidom, source_path, &source) || !FindVoidelleByPath(voidom, destination_path, &destination))
        return -ENOENT;

    Voidelle dest_parent;
    Voidelle source_parent;
    FindParentVoidelleByPath(voidom, source_path, &source_parent);
    FindParentVoidelleByPath(voidom, destination_path, &dest_parent);

    swap_voidelles(voidom, &source, &destination, 0x1);

    return 0;
}

Voidelle getvoidelle(uint64_t pos)
{
    Voidom voidom = GetVoidom();
    Voidelle voidelle;
    read_void(voidom, &voidelle, pos, sizeof(Voidelle));

    return voidelle;
}

int rename_noreplace(const char *source_path, const char *destination_path)
{
    fprintf(stderr, "RENAME NO REPLACE BETWEEN '%s' AND '%s'\n", source_path, destination_path);

    Voidom voidom = GetVoidom();

    if (FindVoidelleByPath(voidom, destination_path, 0))
        return -EEXIST;

    Voidelle dest_parent;
    if (!FindParentVoidelleByPath(voidom, destination_path, &dest_parent))
        return -ENOENT;

    Voidelle source;
    Voidelle source_parent;

    if (!FindVoidelleByPath(voidom, source_path, &source))
        return -ENOENT;
    FindParentVoidelleByPath(voidom, source_path, &source_parent);

    if (source_parent.position != dest_parent.position)
        remove_voidelle(voidom, &source_parent, source, false);

    // in case remove_voidelle changes destination parent, as it can be a neighbour
    read_void(voidom, &dest_parent, dest_parent.position, sizeof(Voidelle));

    clear_voidelle_name(voidom, &source);

    Voidite name_voidite;
    const char *name = GetFilename(destination_path);
    size_t name_len = strlen(name) + 1;

    fprintf(stderr, "RENAMING TO: '%s' WITH %lu CHARS\n", name, name_len);

    populate_voidite_data(voidom, &name_voidite, name, name_len);

    source.name_voidelle = name_voidite.position;
    source.name_voidelle_size = name_len;
    write_void(voidom, &source, source.position, sizeof(Voidelle));

    if (source_parent.position != dest_parent.position)
        add_voidelle(voidom, &dest_parent, &source);

    return 0;
}

int rename(const char *source_path, const char *destination_path)
{
    fprintf(stderr, "RENAME BETWEEN '%s' AND '%s'\n", source_path, destination_path);

    Voidom voidom = GetVoidom();

    if (FindVoidelleByPath(voidom, destination_path, 0))
        return rename_exchange(source_path, destination_path);

    return rename_noreplace(source_path, destination_path);
}

int FuseRename(const char *source_path, const char *destination_path, unsigned int flags)
{
    if (flags == RENAME_EXCHANGE)
        return rename_exchange(source_path, destination_path);
    if (flags == RENAME_NOREPLACE)
        return rename_noreplace(source_path, destination_path);

    return rename(source_path, destination_path);
}