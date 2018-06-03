CC=g++
CFLAGS=-Wall -pedantic -W

all: ipk-lookup

ipk-lookup: ipk-lookup.c
	$(CC) $(CFLAGS) ipk-lookup.c -o ipk-lookup

clean:							 	
	rm -f *~
	rm -f *.o
	rm -f ipk-lookup
