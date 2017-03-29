#include "stream.h"
#include "SQLfunctions.h"

#define MAX_QUERY 512
#define HOSTNAME  "dursley.socs.uoguelph.ca"
#define USERNAME  "nesbittr"
#define PASSWORD  "0915819"
#define DATABASE  "nesbittr"

void updateStream(struct userPost *st) {
    MYSQL mysql;
	mysql_init(&mysql);
	mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&mysql, HOSTNAME, USERNAME, PASSWORD,
		DATABASE, 0, NULL, 0)) {
		printf("Could not connect to host!\n");
		exit(-1);
	}
    addPost(&mysql, st->streamname, st->date, st->username, st->text);
    mysql_close(&mysql);
}

void addUser(char *username, char *list) {
    MYSQL mysql;
	mysql_init(&mysql);
	mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&mysql, HOSTNAME, USERNAME, PASSWORD,
		DATABASE, 0, NULL, 0)) {
		printf("Could not connect to host!\n");
		exit(-1);
	}
    char * token = strtok(list, ",");
    while(token != NULL) {
        addAuthor(&mysql, "add", token, username);
        token = strtok(NULL, ",");
    }
    mysql_close(&mysql);
}

void removeUser(char *username, char *list) {
    MYSQL mysql;
	mysql_init(&mysql);
	mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&mysql, HOSTNAME, USERNAME, PASSWORD,
		DATABASE, 0, NULL, 0)) {
		printf("Could not connect to host!\n");
		exit(-1);
	}
    char * token = strtok(list, ",");
    while(token != NULL) {
        addAuthor(&mysql, "remove", token, username);
        token = strtok(NULL, ",");
    }
    mysql_close(&mysql);
}
