#include "SQLfunctions.h"

#define MAX_QUERY 512
#define HOSTNAME  "dursley.socs.uoguelph.ca"
#define USERNAME  "nesbittr"
#define PASSWORD  "0915819"
#define DATABASE  "nesbittr"

int main(int argc, char *argv[]) {
	/* Opening connection to server */
	MYSQL mysql;
	mysql_init(&mysql);
	mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&mysql, HOSTNAME, USERNAME, PASSWORD,
		DATABASE, 0, NULL, 0)) {
		printf("Could not connect to host!\n");
		exit(-1);
	}
	int j = 1;
	if(argv[j] == NULL) return(-1);
	while(argv[j] != NULL) {
		if(strcmp(argv[j], "-clear") == 0) {
			clearTables(&mysql);
		} else if(strcmp(argv[j], "-reset") == 0) {
			resetDatabase(&mysql);
		} else if(strcmp(argv[j], "-posts") == 0) {
			printPosts(&mysql);
		} else if(strcmp(argv[j], "-users") == 0) {
			printUsers(&mysql);
		} else if(strcmp(argv[j], "-streams") == 0) {
			printStreams(&mysql);
		} else if(strcmp(argv[j], "-help") == 0) {
			printUsage();
		} else if(strcmp(argv[j], "-addauthor") == 0) {
			if(argv[j+1] == NULL || argv[j+2] == NULL || argv[j+3] == NULL) {
				printf("Inproper Usage...\n");
			} else {
				char username[64];
				clrstr(username);
				strcpy(username, argv[j+3]);
				addAuthor(&mysql, argv[j+1], argv[j+2], username);
				j = 4;
			}
		} else if(strcmp(argv[j], "-addpost") == 0) {
			char date[64], username[64], message[255];
			int l = 2;
			strcpy(date, argv[j+l]);
			++l;
			strcpy(username, argv[j+l]);
			++l;
			strcpy(message, argv[j+l]);
			addPost(&mysql, argv[j+1], date, username, message);
			j = l + 1;
		} else {
			printf("Invalid command, use -help for list of valid commands\n");
		}
		++j;
	}
	return(0);
}
