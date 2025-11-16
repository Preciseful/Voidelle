FILES = main.c Cli/cli.c Filesystem/voidelle.c
FILES += $(wildcard Cli/fuse/*.c)

all: build

clean: voidelle
	rm voidelle

build: $(FILES)
	gcc -I. -I/usr/include/ $(FILES) -o voidelle -lfuse3

disk:
	dd if=/dev/zero of=data.bin bs=1G count=2
