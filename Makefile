CC = gcc
CFLAGS = -Wall
.PHONY: all clean run build

all: build run clean

build: $(wildcard *.c)
	$(CC) $(CFLAGS) $^ -o program
run:
	./program
clean:
	rm -rf program
