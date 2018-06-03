HEADERS = isashark.h 
OBJECTS = isashark.o

default: isashark

%.o: $(HEADERS) %.c
	gcc -c $< -o $@ -L/usr/lib -lpcap -D_BSD_SOURCE

isashark: $(OBJECTS)
	gcc $(OBJECTS) -o $@ -L/usr/lib -lpcap -D_BSD_SOURCE

clean:
	-rm -f $(OBJECTS)
	-rm -f isashark
