default: build

build:
	$(CROSS_COMPILE)gcc -g -Wall -c -o writer.o writer.c 
	$(CROSS_COMPILE)gcc writer.o -o writer

clean:
	rm -f *.o
	rm -f writer

rmtest:
	rm -f testfile

