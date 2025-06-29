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

- `identifier` (4 bytes): the header identifier. It should spell out `VOID`.
- `void_size` (8 bytes): this is the maximum size of each void. By default it is `512`. Padding done on each void is according to this value.
- `voidmap_size` (8 bytes): the size of the voidmap.
- `voidmap` (8 bytes): the position of the voidmap.
    - The voidmap is a bitmap that determines whether a void is occupied or not. The first two bits are always marked as occupied (`1`), as `voidlet` and the root take it up.
</details>

### Voidelle

This is the basic entry for files and directories. It contains metadata such as creation/modification timestamps, a header to ensure its integrity and permissions/owner. </br>

<details>
    <summary>Explanation and order of members</summary>

- `velle` (5 bytes): the header. It should spell out `VELLE`.
- `flags` (8 bytes):
    - `0x1`: directory.
    - `0x2`: hidden.
    - `0x4`: system-reserved entry.
    - `0x8`: deleted/invalid entry.
    - An OS can choose to use the remaining bits for its own flags.
- `name` (8 bytes): contains the name's `voidite` position.
- `content` (8 bytes): contains the content's position.
    - the content presented can be a `voidite` or `voidelle`.
- `content_size` (8 bytes): the total size of the current entry's content.
- `next` (8 bytes): the neighbouring `voidelle`.
- `pos` (8 bytes): the current entry's position.
- `create_year` (8 bytes): the year of the current entry's creation.
- `create_date` (5 bytes): each byte represents a certain timestamp value.
    - The order of bytes is `MONTH | DAY | HOUR | MINUTE | SECOND`.
    - The date values can be left as 0.
- `modify_year` (8 bytes): the year of the current entry's last modification.
- `modify_date` (5 bytes): each byte represents a certain timestamp value.
    - The order of bytes is `MONTH | DAY | HOUR | MINUTE | SECOND`.
    - The date values can be left as 0.
- `owner_id` (8 bytes): the ID of the current entry's owner.
    - A superuser's ID could be 0 as an example.
- `others_permission` (1 byte): the permissions of other users other than the owner.
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

- `pos` (8 bytes): the position of the current `voidite`.
- `next` (8 bytes): the position of the next `voidite`.
    - A `voidite` can use the `next` member in order to expand on the content presented if it does not fit in the data's size.
- `data` (`void_size - 16` bytes): the data being held.
</details>


## This tool
It is meant to be used as a way to interact with a voidelle filesystem.

### Build
1. Clone the repository.
2. Run `make`.
> [!CAUTION]
> Make sure the disk you're using is not of essential use as the initialization command will corrupt existing data.
3. Run `./voidelle <DISK> init` in order to initialize the disk.
    - `<DISK>` is the disk you want to mount the filesystem on. Voidelle may need `sudo` to interact with the disk.
4. Run `./voidelle <DISK> ls` to ensure it worked.

### Usage
This section exists as there is no `--help` commands in the tool yet.
Usage: `voidelle <DISK> [COMMAND]`

- `DISK`:
    - The disk to interact with.


> [!TIP]
>`>` is a special character in bash and other shells. If `>` is your root character, run with `\>` instead.

- `COMMAND`:
    - `init`:
        - Initializes the disk.
    - `ls [OPTIONS...] [DIRECTORY...]`:
        - Displays the entries in `DIRECTORY`.
        - `DIRECTORY` must be absolute (start with >).
        - `OPTIONS`:
            - `-l`: long mode. Shows more comprehensive data on the entries.
    - `tree [DIRECTORY...]`:
        - Displays the entries in `DIRECTORY` in a tree format.
        - `DIRECTORY` must be absolute (start with >).
    - `touch [PATH...]`:
        - Creates files in the `PATH` provided.
        - The creation is not recursive.
        - `PATH` must be absolute (start with >).
    - `mkdir [PATH...]`:
        - Creates directories in the `PATH` provided.
        - The creation is not recursive.
        - `PATH` must be absolute (start with >).

### Future progress
- Introducing more `COMMAND` options:
    - `rm [PATH...]`:
        - Removes the file in the `PATH` provided.
        - `PATH` must be absolute (start with >).
    - `rmdir [OPTIONS...] [PATH...]`:
        - Removes the directory in the `PATH` provided.
        - `PATH` must be absolute (start with >).
        - `OPTIONS`:
            - `-r`: recursively deletes everything inside the directory too.