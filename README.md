# Voidelle Filesystem

Voidelle is meant to be a simple to implement filesystem, without focusing on disk or speed efficiency.

## Design
Voidelle revolves around a linked list design to represent data.
> [!NOTE]
> All positions must be absolute relative to the partition.

For example, a directory listing such as
```
>
| etc
| | files
| | environment
```
- `etc` is a `voidelle` with the `directory` flag.
    - the `next` member is set to 0 as there is no following entry.
    - the `content` member is set to the `files` position.
- `files` is a `voidelle` with the `directory` flag.
    - the `next` member is set to the `environment` position.
    - the `content` member is set to 0 as it does not contain any content.
- `environment` is a `voidelle` with no special flags.
    - the `next` member is set to 0 as there is no following entry.
    - the `content` member is set to 0 as it does not contain any content.

## Types of entries

### Voidlet
> [!NOTE]
> The term `void` is used as an umbrella term for all types of entries.

This is the header of the current mounted filesystem. It contains crucial metadata such as the `voidmap`'s position and a void's maximum size. It is also used to identify a voidelle filesystem.
> [!WARNING]
> Voidlet must always be placed at the beginning of the current partition!

<details>
    <summary>Explanation and order of members</summary>

- `header` (4 bytes): It should spell out `VOID`.
- `void_size` (8 bytes): this is the maximum size of each void. By default it is `512`. Padding done on each void is according to this value.
- `voidmap_size` (8 bytes): the size of the voidmap.
- `voidmap` (8 bytes): the position of the voidmap.
    - The voidmap is a bitmap that determines whether a void is occupied or not. The first two bits are always marked as occupied (`1`), as `voidlet` and the root take it up.
</details>

### Voidelle

This is the basic entry for files and directories. It contains metadata such as creation/modification timestamps, a header to ensure its integrity and permissions/owner. </br>

<details>
    <summary>Explanation and order of members</summary>

- `header` (5 bytes): It should spell out `VELLE`.
- `flags` (8 bytes):
    - `0x1`: directory.
    - `0x2`: hidden.
    - `0x4`: system-reserved entry.
    - `0x8`: deleted/invalid entry (rarely used).
    - An OS can choose to use the remaining bits for its own flags.
- `name_voidelle` (8 bytes): contains the name's `voidite` position.
    - Names should include the null terminator.
- `name_voidelle_size` (8 bytes): the total size of the current entry's name + the null terminator.
- `content_voidelle` (8 bytes): contains the content's position.
    - the content presented can be a `voidite` or `voidelle`.
- `content_voidelle_size` (8 bytes): the total size of the current entry's content.
- `next_voidelle` (8 bytes): the neighbouring `voidelle`.
- `position` (8 bytes): the current entry's position.
- `creation_seconds` (8 bytes): epoch seconds since it's creation.
- `modification_seconds` (8 bytes): epoch seconds since it's last modification.
- `access_seconds` (8 bytes): epoch seconds since it's last access.
- `owner_id` (8 bytes): the ID of the current entry's owner.
    - A superuser's ID could be 0 as an example.
- `other_permission` (1 byte): the permissions of other users other than the owner.
    - The usual values are:
        - `0x4`: write permissions.
        - `0x2`: read permissions.
        - `0x1`: execute permissions.
        - `0x7`: all of above.
    - An OS can choose to use the rest of bits for its own purposes.
- `owner_permission` (1 byte): the permissions of the owner.
    - The usual values are similar to `others_permission`.
    - An OS can choose to use the rest of bits for its own purposes.
</details>

### Voidite
This is the structure of any content required by `voidelle`. It is used primarily for the name, as well as for a file's content.

<details>
    <summary>Explanation and order of members</summary>

- `position` (8 bytes): the position of the current `voidite`.
- `next_voidite` (8 bytes): the position of the next `voidite`.
    - A `voidite` uses the `next` member in order to expand on the content presented if it does not fit in the data's size.
- `data` (`void_size - 16` bytes): the data being held.
</details>


## This tool
It is meant to be used as a way to interact with a voidelle filesystem.
The `Filesystem` folder contains a drop-in for baremetal kernels. (certain modifications may be necessary)

### Build
1. Clone the repository.
2. Run `make`.
> [!CAUTION]
> Make sure the disk you're using is not of essential use as the initialization command will corrupt existing data.
3. Run `sudo chown $USER <DISK>`
    - `<DISK>` is the disk you want to initialize the filesystem on. This is a required step as to avoid `sudo` later on.
4. Run `./voidelle <MOUNT_POSITION> --init --disk=<DISK> --user=$(id -u)` in order to initialize the disk.
    - `<DISK>` is the disk you want to initialize the filesystem on.
    - `<MOUNT_POSITION>` is where fuse should mount the disk. You can create a `mnt` folder in your home directory.
    - The argument `--user` sets the owner UID of files. `id -u` gets your current UID.
5. In order to mount, run `./voidelle <MOUNT_POSITION> --disk=<DISK> --user=$(id -u)`
    - `<MOUNT_POSITION>`, `<DISK>` and the UID should remain the same as in the init command.
    - The command can be ran with `-f -d` for debugging purposes.