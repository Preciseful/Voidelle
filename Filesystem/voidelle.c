#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "voidelle.h"

// ALL disk_* functions should be swapped for your disk handling

// seek is relative to the partition's start
void disk_seek(Voidom voidom, uint64_t seek)
{
    fseek(voidom.disk, seek, SEEK_SET);
}

void disk_read(Voidom voidom, void *buf, uint64_t size)
{
    fread(buf, size, 1, voidom.disk);
}

void disk_write(Voidom voidom, void *buf, uint64_t size)
{
    fwrite(buf, size, 1, voidom.disk);
}

long disk_size(Voidom voidom)
{
    fseek(voidom.disk, 0, SEEK_END);
    long file_size = ftell(voidom.disk);

    return file_size;
}

uint64_t get_free_void(Voidom voidom)
{
    uint8_t total_bits[VOID_SIZE];

    for (uint64_t i = 0; i < voidom.voidlet.voidmap_size; i++)
    {
        if (i % VOID_SIZE == 0)
        {
            disk_seek(voidom, voidom.voidlet.voidmap + (i / VOID_SIZE));
            disk_read(voidom, total_bits, VOID_SIZE);
        }

        uint8_t bits = total_bits[i % VOID_SIZE];

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
        total_bits[i % VOID_SIZE] = bits;

        disk_seek(voidom, voidom.voidlet.voidmap + (i / VOID_SIZE));
        disk_write(voidom, total_bits, VOID_SIZE);

        return pos;
    }

    return 0;
}

void invalidate_section(Voidom voidom, uint64_t pos)
{
    uint64_t section = pos / VOID_SIZE;
    uint64_t i = section / 8;
    uint64_t sector_offset = (i / VOID_SIZE) * VOID_SIZE;
    int bit_pos = 7 - (section % 8);

    uint8_t buf[VOID_SIZE];
    disk_seek(voidom, voidom.voidlet.voidmap + sector_offset);
    disk_read(voidom, buf, VOID_SIZE);

    buf[i % VOID_SIZE] &= ~(1 << bit_pos);

    disk_seek(voidom, voidom.voidlet.voidmap + sector_offset);
    disk_write(voidom, buf, VOID_SIZE);
}

bool write_void(Voidom voidom, void *buf, uint64_t position, uint64_t size)
{
    if (size > VOID_SIZE)
        return false;

    uint8_t cpy_buf[VOID_SIZE];
    memset(cpy_buf, 0, VOID_SIZE);
    memcpy(cpy_buf, buf, size);

    disk_seek(voidom, position);
    disk_write(voidom, cpy_buf, VOID_SIZE);

    return true;
}

bool read_void(Voidom voidom, void *buf, uint64_t position, uint64_t size)
{
    if (size > VOID_SIZE)
        return false;

    disk_seek(voidom, position);
    disk_read(voidom, buf, size);
    return true;
}

uint64_t populate_data(Voidom voidom, Voidite *voidite, const void *data, uint64_t size)
{
    uint64_t initial_size = size;
    uint64_t void_count = ((size + VOIDITE_CONTENT_SIZE - 1) / VOIDITE_CONTENT_SIZE) + 1;
    Voidite *sections = malloc(sizeof(Voidite) * void_count);

    for (uint64_t i = 0; i < void_count; i++)
    {
        uint64_t pos = get_free_void(voidom);

        Voidite voidite;
        voidite.position = pos;
        voidite.next_voidite = 0;

        uint64_t bytes = size < VOIDITE_CONTENT_SIZE ? size : VOIDITE_CONTENT_SIZE;
        memcpy(voidite.data, data, bytes);

        data += bytes;
        size -= bytes;

        sections[i] = voidite;

        if (size == 0)
        {
            void_count = i + 1;
            break;
        }
    }

    for (uint64_t i = 0; i < void_count; i++)
    {
        if (i < void_count - 1)
            sections[i].next_voidite = sections[i + 1].position;

        write_void(voidom, &sections[i], sections[i].position, sizeof(Voidite));
    }

    *voidite = sections[0];
    free(sections);
    return initial_size - size;
}

verror_t create_voidlet(Voidom *voidom)
{
    if (!voidom->disk)
        return INVALID_DISK;

    long file_size = disk_size(*voidom);
    long voidmap_size = (file_size / VOID_SIZE + 7) / 8;
    long bitmap_offset = file_size - voidmap_size;
    long voidmap = bitmap_offset - bitmap_offset % VOID_SIZE;

    Voidlet voidlet;
    memcpy(voidlet.header, "VOID", 4);
    voidlet.void_size = VOID_SIZE;
    voidlet.voidmap_size = voidmap_size;
    voidlet.voidmap = voidmap;

    if (!write_void(*voidom, &voidlet, 0, sizeof(Voidlet)))
        return UNKNOWN_ERROR;

    voidom->voidlet = voidlet;

    uint8_t zero_buf[VOID_SIZE];
    memset(zero_buf, 0, VOID_SIZE);
    uint8_t first_bits = 0b10000000;

    for (uint64_t i = 0; i < voidmap_size; i += VOID_SIZE)
        write_void(*voidom, zero_buf, voidmap + i, VOID_SIZE);

    write_void(*voidom, &first_bits, voidmap, sizeof(uint8_t));

    return SUCCESS;
}

verror_t create_voidelle(Voidom voidom, Voidelle *buf, const char *name, enum Voidelle_Flags flags, uint8_t owner_perm, uint8_t other_perm)
{
    uint64_t voidelle_position = get_free_void(voidom);
    time_t t = time(0);

    Voidelle voidelle;
    memcpy(voidelle.header, "VELLE", 5);
    voidelle.flags = flags;
    voidelle.name_voidelle = get_free_void(voidom);
    voidelle.name_voidelle_size = strlen(name) + 1;
    voidelle.content_voidelle = 0;
    voidelle.content_voidelle_size = 0;
    voidelle.next_voidelle = 0;
    voidelle.position = voidelle_position;
    voidelle.creation_seconds = t;
    voidelle.modification_seconds = t;
    voidelle.access_seconds = t;
    voidelle.owner_id = 0;
    voidelle.other_permission = other_perm;
    voidelle.owner_permission = owner_perm;

    if (!write_void(voidom, &voidelle, voidelle.position, sizeof(Voidelle)))
        return UNKNOWN_ERROR;

    Voidite name_voidite;
    name_voidite.position = voidelle.name_voidelle;
    name_voidite.next_voidite = 0;
    memcpy(name_voidite.data, name, voidelle.name_voidelle_size);

    if (!write_void(voidom, &name_voidite, name_voidite.position, sizeof(Voidite)))
        return UNKNOWN_ERROR;

    memcpy(buf, &voidelle, sizeof(voidelle));
    return SUCCESS;
}

verror_t get_voidelle_name(Voidom voidom, Voidelle voidelle, char *buf)
{
    uint64_t read_bytes = 0;
    uint64_t size = voidelle.name_voidelle_size;
    uint64_t pos = voidelle.name_voidelle;

    while (pos)
    {
        uint64_t aligned_size = size > VOIDITE_CONTENT_SIZE ? VOIDITE_CONTENT_SIZE : size;

        Voidite voidite;
        read_void(voidom, &voidite, pos, sizeof(Voidite));
        memcpy(buf + read_bytes, voidite.data, aligned_size);

        size -= aligned_size;
        read_bytes += aligned_size;

        pos = voidite.next_voidite;
    }

    return SUCCESS;
}

bool get_voidite_at(Voidom voidom, Voidelle voidelle, Voidite *buf, unsigned long index)
{
    uint64_t content_pos = voidelle.content_voidelle;

    for (unsigned long i = 0; i < index; i++)
    {
        if (content_pos == 0)
            return false;

        Voidite current_voidite;
        read_void(voidom, &current_voidite, content_pos, sizeof(Voidite));

        content_pos = current_voidite.next_voidite;
    }

    if (content_pos == 0)
        return false;

    read_void(voidom, buf, content_pos, sizeof(Voidite));
    return true;
}

unsigned long read_voidelle(Voidom voidom, Voidelle voidelle, unsigned long seek, void *buf, unsigned long size)
{
    if (voidelle.flags & VOIDELLE_DIRECTORY)
        return FILE_IS_DIRECTORY;

    uint8_t *current_buf = buf;

    uint64_t first_voidite = seek / VOIDITE_CONTENT_SIZE;
    uint64_t last_voidite = (seek + size - 1) / VOIDITE_CONTENT_SIZE;
    uint64_t start = seek % VOIDITE_CONTENT_SIZE;
    uint64_t end = (seek + size) % VOIDITE_CONTENT_SIZE;

    uint64_t bytes_left = size;

    for (unsigned long voidite_index = first_voidite; voidite_index <= last_voidite; voidite_index++)
    {
        Voidite voidite;

        if (!get_voidite_at(voidom, voidelle, &voidite, voidite_index))
            return current_buf - (uint8_t *)buf;

        uint8_t *cpy_start = voidite.data;
        uint64_t cpy_len = VOIDITE_CONTENT_SIZE;

        if (voidite_index == first_voidite)
        {
            cpy_start += start;
            cpy_len -= start;
        }

        if (voidite_index == last_voidite && end != 0)
            // edge case
            cpy_len = end;

        if (cpy_len > bytes_left)
            cpy_len = bytes_left;

        memcpy(current_buf, cpy_start, cpy_len);

        current_buf += cpy_len;
        bytes_left -= cpy_len;
    }

    return current_buf - (uint8_t *)buf;
}