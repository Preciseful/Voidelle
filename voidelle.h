#pragma once

#include <stdint.h>
#include <assert.h>

#define VOIDELLE_ROOT_CHARACTER '/'
#define VOID_SIZE 512
#define VOIDITE_CONTENT_SIZE (VOID_SIZE - sizeof(uint64_t) * 2)

enum Voidelle_Entry_Flags
{
    VOIDELLE_DIRECTORY = 0b1,
    VOIDELLE_HIDDEN = 0b10,
    VOIDELLE_SYSTEM = 0b100,
    VOIDELLE_DELETED = 0b1000,
};

typedef struct voidlet
{
    uint8_t identifier[4];
    uint64_t void_size;
    uint64_t voidmap_size;
    uint64_t voidmap;
} __attribute__((packed)) voidlet_t;

typedef struct voidite
{
    uint64_t pos;
    uint64_t next;
    uint8_t data[VOIDITE_CONTENT_SIZE];
} __attribute__((packed)) voidite_t;

typedef struct voidelle
{
    uint8_t velle[5];
    uint64_t flags;
    uint64_t name;
    uint64_t name_size;
    uint64_t content;
    uint64_t content_size;
    uint64_t next;
    uint64_t pos;
    uint64_t create_year;
    uint8_t create_date[5];
    uint64_t modify_year;
    uint8_t modify_date[5];
    uint64_t owner_id;
    uint8_t others_permission;
    uint8_t owner_permission;
} __attribute__((packed)) voidelle_t;

static_assert(VOID_SIZE > sizeof(voidelle_t) * 2, "VOID_SIZE is too low! Must be higher than a voidelle.");
