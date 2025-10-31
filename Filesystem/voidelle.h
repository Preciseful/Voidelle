#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define VOID_SIZE 512
#define VOIDITE_CONTENT_SIZE (VOID_SIZE - sizeof(uint64_t) * 2)

// All void positions are partition-relative

typedef uint64_t verror_t;
// disk_t should be changed to your proper disk struct
typedef FILE *disk_t;

enum Voidelle_Errors
{
    SUCCESS,
    INVALID_DISK,
    UNKNOWN_ERROR,
    FILE_IS_DIRECTORY,
};

enum Voidelle_Flags
{
    VOIDELLE_DIRECTORY = 0x1,
    VOIDELLE_HIDDEN = 0x2,
    VOIDELLE_SYSTEM = 0x4,
    // not used, but specified
    VOIDELLE_INVALID = 0x8,
};

enum Permission_Flags
{
    PERMISSION_EXECUTE = 0x1,
    PERMISSION_READ = 0x2,
    PERMISSION_WRITE = 0x4,
    PERMISSION_ALL = 0x7,
};

typedef struct __attribute__((packed)) Voidelle
{
    // Check for 'VELLE' to ensure integrity
    uint8_t header[5];
    uint64_t flags;
    uint64_t name_voidelle, name_voidelle_size;
    uint64_t content_voidelle, content_voidelle_size;
    uint64_t next_voidelle;
    uint64_t position;
    uint64_t creation_seconds;
    uint64_t modification_seconds;
    uint64_t access_seconds;
    uint64_t owner_id;
    uint8_t other_permission;
    uint8_t owner_permission;
} Voidelle;

_Static_assert(sizeof(Voidelle) <= VOID_SIZE, "Void size must be greater than a voidelle's size.");

typedef struct __attribute__((packed)) Voidlet
{
    // Check for 'VOID' to ensure integrity or recognize filesystem
    uint8_t header[4];
    uint64_t void_size;
    uint64_t voidmap_size;
    uint64_t voidmap;
} Voidlet;

typedef struct __attribute__((packed)) Voidite
{
    uint64_t position;
    uint64_t next_voidite;
    uint8_t data[VOIDITE_CONTENT_SIZE];
} Voidite;

// Simply a helper struct of useful metadata, it is not placed on the disk
typedef struct __attribute__((packed)) Voidom
{
    disk_t disk;
    Voidlet voidlet;
    Voidelle root;
} Voidom;

bool write_void(Voidom voidom, void *buf, uint64_t position, uint64_t size);
bool read_void(Voidom voidom, void *buf, uint64_t position, uint64_t size);
uint64_t get_free_void(Voidom voidom);
verror_t create_voidlet(Voidom *voidom);
verror_t create_voidelle(Voidom voidom, Voidelle *buf, const char *name, enum Voidelle_Flags flags, uint8_t owner_perm, uint8_t other_perm);
verror_t get_voidelle_name(Voidom voidom, Voidelle voidelle, char *buf);
bool get_voidite_at(Voidom voidom, Voidelle voidelle, Voidite *buf, unsigned long index);
unsigned long read_voidelle(Voidom voidom, Voidelle voidelle, unsigned long seek, void *buf, unsigned long size);