CC=gcc
CFLAGS= -Wall -pedantic -W

all: client server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

server: server.c
	$(CC) $(CFLAGS) server.c -o server

clean:							 	
	rm -f *~
	rm -f *.o
	rm -f client
	rm -f server
