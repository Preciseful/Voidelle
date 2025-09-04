FILES=main.c commands.c
CC=gcc
COPS=-g

all: build

build:
	@$(CC) $(COPS) $(FILES) -o voidelle

clean: voidelle
	@rm voidelle
