CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic 

PROGRAMS=proj2

all: $(PROGRAMS) 
proj2: proj2.c
	gcc $(CFLAGS) -o $@ $< -g -lrt -pthread

clean:
	rm -f $(PROGRAMS)


