CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic 

PROGRAMS=childCenter

all: $(PROGRAMS) 
childCenter: childCenter.c
	gcc $(CFLAGS) -o $@ $< -g -lrt -pthread

clean:
	rm -f $(PROGRAMS)


