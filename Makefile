FILES = main.c

all: build

build: $(FILES)
	gcc -I. $(FILES) -o voidelle
