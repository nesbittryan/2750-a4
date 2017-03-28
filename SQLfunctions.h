#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

void clrstr(char *buf);

void printUsage();

void clearTables(MYSQL *mysql);

void resetDatabase(MYSQL *mysql);

void printPosts(MYSQL *mysql);

void printUsers(MYSQL *mysql);

void printStreams(MYSQL *mysql);

void addAuthor(MYSQL *mysql, char *flag, char *streamname, char *username);

void addPost(MYSQL *mysql, char *date, char *streamname, char *username, char *message);
