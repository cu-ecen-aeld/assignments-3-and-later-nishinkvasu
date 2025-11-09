GCC ?= gcc

all: build

default: build

build:
	$(GCC) -g -Wall -Werror -c -o writer.o writer.c
	$(GCC) writer.o -o writer

clean:
	rm -f *.o
	rm -f writer

rmtest:
	rm -f testfile

