FILES=main.c

all: build run

build:
	gcc $(FILES) -o voidelle

run: voidelle
	./voidelle

clean: voidelle
	rm voidelle