#include <Cli/cli.h>

void init_filesystem(disk_t disk)
{
    Voidom voidom;
    Voidelle root;

    voidom.disk = disk;
    create_voidlet(&voidom);
    create_voidelle(voidom, &root, "/", VOIDELLE_DIRECTORY);

    voidom.root = root;

    printf("Initialized filesystem.\n");
}