# Makefile for aesdsocket server

all: clean build
default: build

build:
	gcc -g -Wall -Werror -c -o aesdsocket.o aesdsocket.c
	gcc aesdsocket.o -o aesdsocket

clean:
	rm -f *.o
	rm -f aesdsocket
