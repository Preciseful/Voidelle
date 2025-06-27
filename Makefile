FILES=main.c commands.c
CC=gcc
COPS=-g
DISK=/dev/sdc3

all: build run
voidelle: build

build:
	@$(CC) $(COPS) $(FILES) -o voidelle

run: voidelle
	@./voidelle $(DISK) ls \>

clean: voidelle
	@rm voidelle