CC=gcc
CFLAGS = -Wall -g -ansi

all: libstream post addauthor creator messages

post: post.cc libstream.a
	$(CC) $(CFLAGS) converter.c -o c
	./c post.cc
	rm c
	$(CC) $(CFLAGS) post.c -o post.o -c
	$(CC) $(CFLAGS) post.o -o post -L. -lstream
	rm post.c

messages:
	mkdir messages

addauthor: addauthor.c libstream.a
	$(CC) $(CFLAGS) addauthor.c -o addauthor.o -c
	$(CC) $(CFLAGS) addauthor.o -o addauthor -L. -lstream
	rm *.o

libstream: stream.c stream.h
	$(CC) -c stream.c -o stream.o
	ar cr libstream.a stream.o
	rm *.o

creator: siteCreator.c
	$(CC) $(CFLAGS) siteCreator.c -o sc

mydb: mydb.o
	$(CC) $(CFLAGS) mydb.o -o db -lmysqlclient -L/usr/lib/x86_64-linux-gnu/

mydb.o: db.c
	$(CC) $(CFLAGS) db.c -o db.o -c
