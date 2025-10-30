FILES = main.c Cli/init.c Filesystem/voidelle.c

all: build

build: $(FILES)
	gcc -I. $(FILES) -o voidelle

disk:
	dd if=/dev/zero of=data.bin bs=1G count=2
