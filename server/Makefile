# Makefile for aesdsocket server
GCC ?= gcc

all: clean build
default: build

build:
	$(GCC) -g -Wall -Werror -c -o aesdsocket.o aesdsocket.c
	$(GCC) aesdsocket.o -o aesdsocket

clean:
	rm -f *.o
	rm -f aesdsocket
