CC=gcc
CFLAGS=-lWarn -pedantic
LDFLAGS=-lwiringPi

temp:	temp.o libmyifttt.a
	cc temp.o -L. -lmyifttt -lcurl -o temp

libmyifttt.a:	ifttt.o
	ar -rcs libmyifttt.a ifttt.o

ifttt.o:	ifttt.c ifttt.h
	$(CC) $(CFLAGS) -c -ansi $<

temp.o:	temp.c
	$(CC) $(CFLAGS) -c -ansi -std=gnu99 $<

clean:
	rm temp *.o
