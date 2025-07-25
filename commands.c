#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "commands.h"
#include "voidelle.h"

static const char *months[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

FILE *disk = 0;

voidlet_t get_voidlet()
{
    voidlet_t voidlet;
    fseek(disk, 0, SEEK_SET);
    fread(&voidlet, sizeof(voidlet_t), 1, disk);

    return voidlet;
}

voidelle_t get_voidelle(long seek)
{
    if (seek % VOID_SIZE != 0)
        printf("get_voidelle: failed.\n");
    voidelle_t voidelle;
    fseek(disk, seek, SEEK_SET);
    fread(&voidelle, sizeof(voidelle_t), 1, disk);

    return voidelle;
}

char *get_voidelle_name(voidelle_t voidelle)
{
    char *velle_name = malloc(VOID_SIZE);
    velle_name[0] = '\0';
    uint64_t name_pos = voidelle.name;

    for (size_t i = 0; name_pos; i++)
    {
        if (i > 0)
            velle_name = realloc(velle_name, VOID_SIZE * i);

        voidite_t name;
        fseek(disk, name_pos, SEEK_SET);
        fread(&name, 512, 1, disk);
        name_pos = name.next;

        if (name_pos == 0)
            strcpy(velle_name + i * VOID_SIZE, name.data);
        else
            memcpy(velle_name + i * VOID_SIZE, name.data, VOID_SIZE - 2 * sizeof(unsigned long));
    }

    return velle_name;
}

size_t get_entries(voidelle_t voidelle, voidelle_t **b_entries)
{
    if (voidelle.content == 0)
        return 0;

    voidelle_t *entries = malloc(sizeof(voidelle_t));
    size_t entries_capacity = 1;
    size_t entries_count = 0;

    uint64_t entry_pos = voidelle.content;
    for (size_t i = 0; entry_pos; i++)
    {
        if (entries_capacity == i)
        {
            entries = realloc(entries, sizeof(voidelle_t) * entries_capacity * 2);
            entries_capacity *= 2;
        }

        voidelle_t entry = get_voidelle(entry_pos);
        entries[i] = entry;
        entry_pos = entry.next;
        entries_count++;
    }

    *b_entries = entries;
    return entries_count;
}

uint64_t get_free_section()
{
    voidlet_t voidlet = get_voidlet();

    for (size_t i = 0; i < voidlet.voidmap_size; i++)
    {
        unsigned char bits;
        fseek(disk, voidlet.voidmap + i, SEEK_SET);
        fread(&bits, 1, 1, disk);
        int bit_pos = 0;

        if (bits == UINT8_MAX)
            continue;

        if (bits == 0)
        {
            bits = 0b10000000;
            bit_pos = 7;
        }
        else
        {
            bit_pos = __builtin_ctz(bits);
            bit_pos--;
            bits |= (1 << bit_pos);
        }

        uint64_t pos = (i * 8 + (7 - bit_pos)) * VOID_SIZE;
        fseek(disk, voidlet.voidmap + i, SEEK_SET);
        fwrite(&bits, 1, 1, disk);

        return pos;
    }

    return 0;
}

void invalidate_section(uint64_t pos)
{
    voidlet_t voidlet = get_voidlet();

    uint64_t section = pos / VOID_SIZE;
    size_t i = section / 8;
    int bit_pos = 7 - (section % 8);

    unsigned char bits;
    fseek(disk, voidlet.voidmap + i, SEEK_SET);
    fread(&bits, 1, 1, disk);

    bits &= ~(1 << bit_pos);

    fseek(disk, voidlet.voidmap + i, SEEK_SET);
    fwrite(&bits, 1, 1, disk);
}

bool create_voidelle(char *filename, uint64_t flags, voidelle_t *b_voidelle)
{
    voidelle_t voidelle;
    voidelle.pos = get_free_section();

    if (voidelle.pos == 0)
    {
        printf("Failed to get free section.\n");
        return false;
    }

    size_t filename_len = strlen(filename) + 1;
    uint64_t last_pos = 0, init_pos = 0;
    for (size_t i = 0; i < filename_len; i += VOIDITE_CONTENT_SIZE)
    {
        size_t current_len = filename_len - i;
        uint64_t pos = get_free_section();

        if (init_pos == 0)
            init_pos = pos;
        if (last_pos != 0)
        {
            voidite_t last_voidite;
            fseek(disk, last_pos, SEEK_SET);
            fread(&last_voidite, sizeof(voidite_t), 1, disk);

            last_voidite.next = pos;

            fseek(disk, last_pos, SEEK_SET);
            fwrite(&last_voidite, sizeof(voidite_t), 1, disk);
        }

        voidite_t voidite;
        voidite.next = 0;
        voidite.pos = pos;
        memcpy(voidite.data, filename + i, (current_len > VOIDITE_CONTENT_SIZE ? VOIDITE_CONTENT_SIZE : current_len));

        fseek(disk, voidite.pos, SEEK_SET);
        fwrite(&voidite, sizeof(voidite_t), 1, disk);

        last_pos = pos;
    }

    voidite_t voidelle_name;
    fseek(disk, init_pos, SEEK_SET);
    fread(&voidelle_name, sizeof(voidite_t), 1, disk);

    time_t t = time(0);
    struct tm *tm = localtime(&t);
    uint8_t cdate[5] = {(uint8_t)tm->tm_mon, (uint8_t)tm->tm_mday, (uint8_t)tm->tm_hour, (uint8_t)tm->tm_min, (uint8_t)tm->tm_sec};

    voidelle.name = voidelle_name.pos;
    voidelle.next = 0;
    voidelle.content = 0;
    voidelle.content_size = 0;
    voidelle.flags = flags;
    voidelle.owner_id = 0;
    voidelle.owner_permission = 0b111;
    voidelle.others_permission = 0;
    voidelle.create_year = tm->tm_year + 1900;
    voidelle.modify_year = tm->tm_year + 1900;
    memcpy(voidelle.create_date, cdate, sizeof(cdate));
    memcpy(voidelle.modify_date, cdate, sizeof(cdate));
    memcpy(voidelle.velle, "VELLE", 5);

    fseek(disk, voidelle.pos, SEEK_SET);
    fwrite(&voidelle, sizeof(voidelle_t), 1, disk);
    *b_voidelle = voidelle;

    debug("Voidelle %lu & name %lu\n", voidelle.pos, voidelle.name);

    return true;
}

void init()
{
    time_t t = time(0);
    struct tm *tm = localtime(&t);
    uint8_t cdate[5] = {(uint8_t)tm->tm_mon, (uint8_t)tm->tm_mday, (uint8_t)tm->tm_hour, (uint8_t)tm->tm_min, (uint8_t)tm->tm_sec};

    fseek(disk, 0, SEEK_END);
    long file_size = ftell(disk);
    long voidmap_size = (file_size / 512 + 7) / 8;
    long bitmap_offset = file_size - voidmap_size;
    long voidmap = bitmap_offset - bitmap_offset % 512;

    voidlet_t voidlet;
    voidlet.void_size = VOID_SIZE;
    voidlet.voidmap_size = voidmap_size;
    voidlet.voidmap = voidmap;
    memcpy(voidlet.identifier, "VOID", 4);

    debug("Voidmap starts at %lu.\n", voidlet.voidmap);

    voidite_t root_name;
    root_name.pos = VOID_SIZE * 2;
    root_name.next = 0;
    root_name.data[0] = VOIDELLE_ROOT_CHARACTER;
    root_name.data[1] = '\0';

    voidelle_t root;
    root.flags = VOIDELLE_SYSTEM | VOIDELLE_DIRECTORY;
    root.name = root_name.pos;
    root.content = 0;
    root.content_size = 0;
    root.next = 0;
    root.pos = VOID_SIZE;
    root.owner_id = 0;
    root.owner_permission = 0b111;
    root.others_permission = 0;
    root.create_year = tm->tm_year + 1900;
    root.modify_year = tm->tm_year + 1900;
    memcpy(root.create_date, cdate, sizeof(cdate));
    memcpy(root.modify_date, cdate, sizeof(cdate));
    memcpy(root.velle, "VELLE", 5);

    fseek(disk, 0, SEEK_SET);
    fwrite(&voidlet, sizeof(voidlet_t), 1, disk);
    fseek(disk, VOID_SIZE, SEEK_SET);
    fwrite(&root, sizeof(voidelle_t), 1, disk);
    fseek(disk, VOID_SIZE * 2, SEEK_SET);
    fwrite(&root_name, VOID_SIZE, 1, disk);

    unsigned char *zero_buf = calloc(voidmap_size, 1);
    zero_buf[0] = 0b11100000;
    fseek(disk, voidmap, SEEK_SET);
    fwrite(zero_buf, 1, voidmap, disk);
    free(zero_buf);
}

bool get_voidelle_from_path(char *path, voidelle_t *b_voidelle)
{
    debug("Getting voidelle from path: '%s'\n", path);
    if (*path != VOIDELLE_ROOT_CHARACTER)
    {
        printf("Paths must be absolute when using ls.\n");
        return false;
    }

    path++;

    voidelle_t dir = get_voidelle(VOID_SIZE);
    for (size_t start = 0; start < strlen(path);)
    {
        size_t end = start;
        while (path[end] && path[end] != '/')
            end++;

        size_t folder_path_size = end - start;
        char *folder_name = malloc(folder_path_size + 1);
        folder_name[folder_path_size] = 0;
        memcpy(folder_name, path + start, folder_path_size);

        debug("Found subdirectory '%s' within the path. Checking it...\n", folder_name);

        if (dir.content == 0)
        {
            printf("Directory '%s' does not exist.\n", folder_name);
            free(folder_name);
            return false;
        }

        voidelle_t *entries;
        size_t entries_count = get_entries(dir, &entries);
        bool found = false;

        for (size_t i = 0; i < entries_count; i++)
        {
            char *entry_name = get_voidelle_name(entries[i]);
            debug("Checking against '%s'.\n", entry_name);
            if (folder_path_size != strlen(entry_name))
            {
                free(entry_name);
                continue;
            }

            if (memcmp(folder_name, entry_name, folder_path_size) != 0)
            {
                free(entry_name);
                continue;
            }

            free(entry_name);
            dir = entries[i];
            found = true;
            break;
        }

        if (!found)
        {
            printf("Directory '%s' does not exist.\n", folder_name);
            free(folder_name);
            if (entries_count > 0)
                free(entries);
            return false;
        }

        if (!(dir.flags & VOIDELLE_DIRECTORY) && path[end])
        {
            printf("Cannot open files ('%s').\n", folder_name);
            free(folder_name);
            if (entries_count > 0)
                free(entries);
            return false;
        }

        start = end + 1;
        free(folder_name);
        if (entries_count > 0)
            free(entries);
    }

    *b_voidelle = dir;
    return true;
}

void display_entry(voidelle_t velle, size_t level, enum Ls_Options option)
{
    char *velle_name = get_voidelle_name(velle);

    if (option == LS_TREE)
    {
        for (size_t i = 0; i < level; i++)
            printf(" |");
        printf(" ");
    }
    else if (option == LS_LONG)
    {
        printf("%u%u%u",
               (velle.owner_permission << 1) & 1,
               (velle.owner_permission << 2) & 2,
               (velle.owner_permission << 3) & 4);
        printf("%u%u%u",
               (velle.others_permission << 1) & 1,
               (velle.others_permission << 2) & 2,
               (velle.others_permission << 3) & 4);
        printf(" (%lu) ", velle.owner_id);
        printf("%lu %s %u %02u:%02u:%02u ",
               velle.modify_year,
               months[velle.modify_date[0]],
               velle.modify_date[1],
               velle.modify_date[2],
               velle.modify_date[3],
               velle.modify_date[4]);
    }

    if (velle.flags & VOIDELLE_DIRECTORY)
        printf("\e[1;34m%s\e[0m", velle_name);
    else
        printf("%s", velle_name);

    if (option == LS_TREE && ((velle.flags) & VOIDELLE_DIRECTORY))
    {
        printf("\n");
        voidelle_t *entries;
        size_t entries_count = get_entries(velle, &entries);

        for (size_t i = 0; i < entries_count; i++)
            display_entry(entries[i], level + 1, option);
        if (entries_count > 0)
            free(entries);
    }
    else if (option == LS_TREE)
        printf("\n");

    free(velle_name);
}

void ls(char *path, enum Ls_Options flag)
{
    debug("Command ls called with path: '%s'\n", path);

    voidelle_t root;
    if (*path != 0 && !get_voidelle_from_path(path, &root))
    {
        printf("Invalid path.\n");
        return;
    }
    else if (*path == 0)
        root = get_voidelle(VOID_SIZE);

    char *root_name = get_voidelle_name(root);

    if (!(root.flags & VOIDELLE_DIRECTORY))
    {
        printf("Cannot use ls on files ('%s').\n", root_name);
        free(root_name);
        return;
    }

    voidelle_t *entries;
    size_t entries_count = get_entries(root, &entries);

    printf("%s:\n", root_name);
    for (size_t i = 0; i < entries_count; i++)
    {
        debug("Displaying voidelle %lu & name %lu\n", entries[i].pos, entries[i].name);
        display_entry(entries[i], 1, flag);
        if (flag == LS_LONG)
            printf("\n");
        else if (flag == LS_NONE)
            printf(" ");
    }

    if (flag == LS_NONE)
        printf("\n");
    if (entries_count > 0)
        free(entries);
    free(root_name);
}

char *get_filename(char *path, voidelle_t *parent, bool create_parents)
{
    if (path[0] != VOIDELLE_ROOT_CHARACTER)
    {
        printf("Paths must be absolute. ('%s')\n", path);
        return "";
    }

    if (path[1] == '\0')
    {
        printf("Cannot create a root directory. ('%s')\n", path);
        return "";
    }

    voidelle_t dir;
    size_t slash = 0;
    size_t path_len = strlen(path);
    if (path_len == 1)
        return "";

    for (long i = path_len - 1; i >= 1; i--)
    {
        if (path[i] == '/')
        {
            slash = i;
            break;
        }
    }

    if (slash == 0)
        dir = get_voidelle(VOID_SIZE);
    else
    {
        size_t new_path_len = slash;

        char *new_path = malloc(new_path_len + 1);
        memcpy(new_path, path, new_path_len);
        new_path[new_path_len] = 0;

        if (create_parents)
        {
            for (unsigned long i = 0; i <= new_path_len; i++)
            {
                if (new_path[i] == '/' || new_path[i] == 0)
                {
                    new_path[i] = 0;
                    make(new_path, VOIDELLE_DIRECTORY, false);
                    new_path[i] = '/';
                }
            }

            new_path[new_path_len] = 0;
        }

        if (!get_voidelle_from_path(new_path, &dir))
        {
            printf("Directory '%s' does not exist.\n", new_path);
            return "";
        }

        free(new_path);
    }

    if (parent != 0)
        *parent = dir;

    char *filename = path + slash + 1;
    return filename;
}

void make(char *path, uint64_t flags, bool recursive)
{
    voidelle_t dir;
    voidelle_t voidelle;

    char *filename = get_filename(path, &dir, recursive);
    if (filename[0] == '\0')
        return;

    if (!create_voidelle(filename, flags, &voidelle))
    {
        printf("Failed to create a voidelle.\n");
        return;
    }

    if (dir.content == 0)
    {
        dir.content = voidelle.pos;
        fseek(disk, dir.pos, SEEK_SET);
        fwrite(&dir, sizeof(voidelle_t), 1, disk);
    }
    else
    {
        voidelle_t neighbour;
        fseek(disk, dir.content, SEEK_SET);
        fread(&neighbour, sizeof(voidelle_t), 1, disk);

        char *neighbour_name = get_voidelle_name(neighbour);
        if (strcmp(neighbour_name, filename) == 0)
        {
            printf("File %s already exists.\n", filename);
            free(neighbour_name);
            return;
        }

        free(neighbour_name);

        while (neighbour.next)
        {
            fseek(disk, neighbour.next, SEEK_SET);
            fread(&neighbour, sizeof(voidelle_t), 1, disk);

            neighbour_name = get_voidelle_name(neighbour);
            if (strcmp(neighbour_name, filename) == 0)
            {
                printf("File %s already exists.\n", filename);
                free(neighbour_name);
                return;
            }

            free(neighbour_name);
        }

        neighbour.next = voidelle.pos;
        fseek(disk, neighbour.pos, SEEK_SET);
        fwrite(&neighbour, sizeof(voidelle_t), 1, disk);
    }
}

bool rm_voidelle(voidelle_t dir, char *filename, bool ignore_content, voidelle_t *bvoidelle)
{
    voidelle_t child;
    voidelle_t previous_child;
    uint64_t child_pos = dir.content;

    while (child_pos != 0)
    {
        fseek(disk, child_pos, SEEK_SET);
        fread(&child, sizeof(voidelle_t), 1, disk);

        char *child_name = get_voidelle_name(child);
        if (strcmp(get_voidelle_name(child), filename) == 0)
        {
            free(child_name);
            break;
        }

        free(child_name);

        previous_child = child;
        child_pos = child.next;
    }

    if (child_pos == 0)
    {
        printf("File '%s' does not exist.\n", filename);
        return false;
    }

    if ((child.flags & VOIDELLE_DIRECTORY) && child.content != 0 && !ignore_content)
    {
        printf("Directory '%s' is not empty.\n", filename);
        return false;
    }

    if (child_pos == dir.content)
    {
        dir.content = child.next;
        fseek(disk, dir.pos, SEEK_SET);
        fwrite(&dir, sizeof(voidelle_t), 1, disk);
        debug("Removed child from parent directory: %lu.\n", dir.pos);
    }
    else
    {
        previous_child.next = child.next;
        fseek(disk, previous_child.pos, SEEK_SET);
        fwrite(&previous_child, sizeof(voidelle_t), 1, disk);
        debug("Removed child from neighbour: %lu.\n", previous_child.pos);
    }

    uint64_t name_pos = child.name;
    voidite_t name;
    while (name_pos)
    {
        fseek(disk, name_pos, SEEK_SET);
        fread(&name, sizeof(voidite_t), 1, disk);

        invalidate_section(name_pos);
        debug("Invalidating name: %lu.\n", name_pos);
        name_pos = name.next;
    }

    uint64_t content_pos = child.content;
    voidite_t content;
    while (content_pos && !(child.flags & VOIDELLE_DIRECTORY))
    {
        fseek(disk, content_pos, SEEK_SET);
        fread(&content, sizeof(voidite_t), 1, disk);

        invalidate_section(content_pos);
        debug("Invalidating content: %lu.\n", content_pos);
        content_pos = content.next;
    }

    invalidate_section(child.pos);
    debug("Invalidated child: %lu.\n", child.pos);
    *bvoidelle = child;
    return true;
}

void rm_file(char *path, bool recursive)
{
    voidelle_t dir;
    char *filename = get_filename(path, &dir, false);
    if (filename[0] == '\0')
        return;

    voidelle_t parent;
    if (!rm_voidelle(dir, filename, recursive, &parent))
        return;

    if (recursive)
    {
        voidelle_t child;
        uint64_t child_pos = parent.content;
        size_t path_len = strlen(path);

        while (child_pos != 0)
        {
            fseek(disk, child_pos, SEEK_SET);
            fread(&child, sizeof(voidelle_t), 1, disk);

            char *child_name = get_voidelle_name(child);
            char *new_path = malloc(path_len + strlen(child_name) + 2);

            strcpy(new_path, path);
            new_path[path_len] = '/';
            new_path[path_len + 1] = 0;

            strcat(new_path, child_name);

            rm_file(new_path, true);

            free(child_name);
            child_pos = child.next;
        }
    }
}

void write(char *path, char *data)
{
    voidelle_t voidelle;
    if (!get_voidelle_from_path(path, &voidelle))
        return;

    if (voidelle.flags & VOIDELLE_DIRECTORY)
    {
        printf("Cannot write to directory.\n");
        return;
    }

    size_t old_content = voidelle.content;
    while (old_content)
    {
        voidite_t voidite;
        fseek(disk, old_content, SEEK_SET);
        fread(&voidite, sizeof(voidite_t), 1, disk);

        invalidate_section(voidite.pos);
        old_content = voidite.next;
    }

    size_t data_len = strlen(data) + 1;
    uint64_t last_pos = 0, init_pos = 0;

    for (size_t i = 0; i < data_len; i += VOIDITE_CONTENT_SIZE)
    {
        size_t current_len = data_len - i;
        uint64_t pos = get_free_section();

        if (init_pos == 0)
            init_pos = pos;
        if (last_pos != 0)
        {
            voidite_t last_voidite;
            fseek(disk, last_pos, SEEK_SET);
            fread(&last_voidite, sizeof(voidite_t), 1, disk);

            last_voidite.next = pos;

            fseek(disk, last_pos, SEEK_SET);
            fwrite(&last_voidite, sizeof(voidite_t), 1, disk);
        }

        voidite_t voidite;
        voidite.next = 0;
        voidite.pos = pos;
        memcpy(voidite.data, data + i, (current_len > VOIDITE_CONTENT_SIZE ? VOIDITE_CONTENT_SIZE : current_len));

        fseek(disk, voidite.pos, SEEK_SET);
        fwrite(&voidite, sizeof(voidite_t), 1, disk);

        last_pos = pos;
    }

    voidelle.content_size = data_len;
    voidelle.content = init_pos;
    fseek(disk, voidelle.pos, SEEK_SET);
    fwrite(&voidelle, sizeof(voidelle_t), 1, disk);
}

void cat(char *path)
{
    voidelle_t voidelle;
    if (!get_voidelle_from_path(path, &voidelle))
        return;
    if (voidelle.flags & VOIDELLE_DIRECTORY)
    {
        printf("Cannot read contents of directories.\n");
        return;
    }

    uint64_t data_pos = voidelle.content;
    size_t leftover = voidelle.content_size;

    while (data_pos)
    {
        voidite_t data;
        fseek(disk, data_pos, SEEK_SET);
        fread(&data, sizeof(voidite_t), 1, disk);

        for (size_t i = 0; i < (leftover > VOIDITE_CONTENT_SIZE ? VOIDITE_CONTENT_SIZE : leftover); i++)
            printf("%c", data.data[i]);

        leftover -= VOIDITE_CONTENT_SIZE;
        data_pos = data.next;
    }

    printf("\n");
}