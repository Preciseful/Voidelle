FILES=main.c commands.c
COPS=-DDEBUG_RELEASE -g
DISK=/dev/sdc3

all: build run
voidelle: build

build:
	@gcc $(COPS) $(FILES) -o voidelle

run: voidelle
	@./voidelle $(DISK) ls \*

clean: voidelle
	@rm voidelle