CC = gcc
CFLAGS = -Wall
LIBS = -lm
.PHONY: all clean run build

all: build

build: $(wildcard controller/*.c debug/*.c internal/*.c internal/containers/*.c internal/generators/*.c internal/models/*.c *.c)
	$(CC) $(CFLAGS) $^ -o program $(LIBS)
run:
	./program
clean:
	rm -rf program
