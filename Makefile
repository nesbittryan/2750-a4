CC=gcc
CFLAGS = -Wall -g -ansi

all: libstream post addauthor creator mydb

post: post.cc libstream.a
	$(CC) $(CFLAGS) converter.c -o c
	./c post.cc
	rm c
	$(CC) $(CFLAGS) post.c -o post.o -c
	$(CC) post.o -o post -L. -lstream -lmysqlclient -L/usr/lib/x86_64-linux-gnu/
	rm post.c

addauthor: addauthor.c libstream.a
	$(CC) $(CFLAGS) addauthor.c -o addauthor.o -c
	$(CC) addauthor.o -o addauthor -L. -lstream -lmysqlclient -L/usr/lib/x86_64-linux-gnu/
	rm *.o

libstream: stream.c stream.h SQLfunctions.c SQLfunctions.h
	$(CC) -c stream.c -o stream.o
	$(CC) -c SQLfunctions.c -o SQL.o
	ar cr libstream.a stream.o SQL.o
	rm *.o

creator: siteCreator.c
	$(CC) $(CFLAGS) siteCreator.c -o sc

mydb:
	$(CC) $(CFLAGS) db.c -o db -L. -lstream -lmysqlclient -L/usr/lib/x86_64-linux-gnu/
