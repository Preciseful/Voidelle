#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "../cli.h"

int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    Voidelle voidelle;

    struct cli_context *ctx = fuse_get_context()->private_data;
    if (!read_path(ctx->voidom, path, &voidelle, 0))
        return -ENOENT;

    if (!voidelle.content_voidelle)
    {
        Voidite content;
        content.position = get_free_void(ctx->voidom);
        content.next_voidite = 0;
        memset(content.data, 0, VOIDITE_CONTENT_SIZE);

        voidelle.content_voidelle = content.position;

        write_void(ctx->voidom, &voidelle, voidelle.position, sizeof(Voidelle));
        write_void(ctx->voidom, &content, content.position, sizeof(Voidite));
    }

    size_t new_size = offset + size;

    if (new_size > voidelle.content_voidelle_size)
    {
        size_t voidites_count = (new_size + (VOIDITE_CONTENT_SIZE - 1)) / VOIDITE_CONTENT_SIZE;
        Voidite last_voidite;

        voidelle.content_voidelle_size = new_size;
        write_void(ctx->voidom, &voidelle, voidelle.position, sizeof(Voidelle));

        unsigned long pos = voidelle.content_voidelle;
        while (pos)
        {
            read_void(ctx->voidom, &last_voidite, pos, sizeof(Voidite));
            pos = last_voidite.next_voidite;
            voidites_count--;
        }

        for (size_t i = 0; i < voidites_count; i++)
        {
            Voidite next_voidite;
            next_voidite.next_voidite = 0;
            next_voidite.position = get_free_void(ctx->voidom);
            memset(next_voidite.data, 0, VOIDITE_CONTENT_SIZE);

            last_voidite.next_voidite = next_voidite.position;

            write_void(ctx->voidom, &last_voidite, last_voidite.position, sizeof(Voidite));
            write_void(ctx->voidom, &next_voidite, next_voidite.position, sizeof(Voidite));

            last_voidite = next_voidite;
        }
    }

    unsigned long voidite_start = offset / VOIDITE_CONTENT_SIZE;
    unsigned long voidite_offset = offset % VOIDITE_CONTENT_SIZE;
    unsigned long voidite_end = (offset + size) / VOIDITE_CONTENT_SIZE;
    unsigned long voidite_pos = voidite_start;

    int written = 0;
    while (size)
    {
        Voidite voidite;
        get_content_voidite_at(ctx->voidom, voidelle, &voidite, voidite_pos);

        unsigned long bytes_count;

        if (voidite_pos == voidite_start)
            bytes_count = VOIDITE_CONTENT_SIZE - voidite_offset;
        else
            bytes_count = size;

        if (bytes_count > size)
            bytes_count = size;

        memcpy(voidite.data + (voidite_pos == voidite_start ? voidite_offset : 0),
               buf + written, bytes_count);

        write_void(ctx->voidom, &voidite, voidite.position, sizeof(Voidite));

        written += bytes_count;
        size -= bytes_count;

        voidite_pos = voidite.next_voidite;
    }

    return written;
}