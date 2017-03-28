#include "SQLfunctions.h"

#define MAX_QUERY 512

void addPost(MYSQL *mysql, char *streamname, char *date, char *username, char *message) {
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query[MAX_QUERY];
	strcpy(query, "CREATE TABLE IF NOT EXISTS Users (user_name CHAR(64),");
	strcat(query, " stream_name CHAR(64), read_messages DECIMAL(3), PRIMARY KEY (user_name, stream_name))");
	if(mysql_query(mysql, query)){
		printf("Could not create Users\n");
		return;
	}
	clrstr(query);
	strcpy(query, "CREATE TABLE IF NOT EXISTS Messages (stream_name CHAR(64),");
	strcat(query, " date CHAR(64), user_name CHAR(64), message VARCHAR(255))");
	if(mysql_query(mysql, query)){
		printf("Could not create Messages\n");
		return;
	}
	clrstr(query);
	sprintf(query, "SELECT * FROM Users WHERE Users.user_name = '%s' AND Users.stream_name = '%s'", username, streamname);
	if(mysql_query(mysql, query)){
		printf("Could not run query\n");
	}
	if (!(res = mysql_store_result(mysql))){
		printf("%s does not have permission for stream %s\n", username, streamname);
	}
	else {
		if((row = mysql_fetch_row(res))) {
			clrstr(query);
			strcpy(query, " INSERT INTO Messages (stream_name, date, user_name, message) ");
			char temp[MAX_QUERY];
			clrstr(temp);
			sprintf(temp, "VALUES ('%s', '%s', '%s', '%s')", streamname, date, username, message);
			strcat(query, temp);
			if(mysql_query(mysql, query)){
				printf("Could not run query!\n");
			} else {
				printf("Message posted to %s\n", streamname);
			}
		} else {
			printf("%s does not have permission for stream %s\n", username, streamname);
		}
	}
}

void addAuthor(MYSQL *mysql, char *flag, char *streamname, char *username) {
	char query[MAX_QUERY];
	strcpy(query, "CREATE TABLE IF NOT EXISTS Messages (stream_name CHAR(64),");
	strcat(query, " date CHAR(64), user_name CHAR(64), message VARCHAR(255))");
	if(mysql_query(mysql, query)){
		printf("Could not create Messages\n");
		return;
	}
	strcpy(query, "CREATE TABLE IF NOT EXISTS Users (user_name CHAR(64),");
	strcat(query, " stream_name CHAR(64), read_messages DECIMAL(3), PRIMARY KEY (user_name, stream_name))");
	if(mysql_query(mysql, query)){
		printf("Could not create Users\n");
		return;
	}
	clrstr(query);
	if(strcmp(flag, "add") == 0) {
		sprintf(query, "INSERT INTO Users (user_name, stream_name, read_messages) VALUES ('%s','%s',0)", username, streamname);
		if(mysql_query(mysql, query)){
			printf("%s already in stream %s\n", username, streamname);
		} else {
			printf("%s Added to Stream %s\n", username, streamname);
		}
	} else {
		sprintf(query, "DELETE FROM Users where Users.user_name ='%s' AND Users.stream_name = '%s'", username, streamname);
		if(mysql_query(mysql, query)){
			printf("%s not in stream %s\n", username, streamname);
		} else {
			printf("%s Removed from Stream %s\n", username, streamname);
		}
	}
}

void printStreams(MYSQL *mysql) {
	char query[MAX_QUERY];
	MYSQL_RES *res;
	MYSQL_ROW row;
	strcpy(query, "SELECT stream_name FROM Users GROUP BY stream_name");
	if(mysql_query(mysql, query)){
		printf("Could not run query\n");
	}
	if (!(res = mysql_store_result(mysql))){
		printf("No Streams Found!\n");
	} else {
		/* print all results */
		printf("Stream List:\n----------\n");
		while ((row = mysql_fetch_row(res))) {
			printf("%s\n", row[0]);
		}
		printf("----------\n");
	}
}

void printUsers(MYSQL *mysql) {
	char query[MAX_QUERY];
	MYSQL_RES *res;
	MYSQL_ROW row;
	strcpy(query, "SELECT user_name FROM Users GROUP BY user_name");
	if(mysql_query(mysql, query)){
		printf("Could not run query\n");
	}
	if (!(res = mysql_store_result(mysql))){
		printf("No Users Found!\n");
	} else {
		/* print all results */
		printf("Users List:\n----------\n");
		while ((row = mysql_fetch_row(res))) {
			printf("%s\n", row[0]);
		}
		printf("----------\n");
	}

}

void printPosts(MYSQL *mysql) {
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query[MAX_QUERY];
	strcpy(query, "SELECT * FROM Messages");
	if(mysql_query(mysql, query)){
		printf("Could not run query\n");
	}
	if (!(res = mysql_store_result(mysql))){
		printf("No Posts Found!\n");
	} else {
		/* print all results */
		printf("Posts:\n----------\n");
		while ((row = mysql_fetch_row(res))) {
			printf("Stream:%s\nAuthor:%s\nDate:%s\nMessage:%s\n\n", row[0], row[2], row[1], row[3]);
		}
		printf("----------\n");
	}
}

void resetDatabase(MYSQL *mysql) {
	char query[MAX_QUERY];
	strcpy(query, "drop table Users");
	if(mysql_query(mysql, query)){
		printf("Could not find Users\n");
	}
	strcpy(query, "drop table Messages");
	if(mysql_query(mysql, query)){
		printf("Could not find Messages\n");
	}
}

void clearTables(MYSQL *mysql) {
	char query[MAX_QUERY];
	clrstr(query);
	strcpy(query, "truncate Messages");
	if(mysql_query(mysql, query)){
		printf("Could not find Messages\n");
	}
	strcpy(query, "truncate Users");
	if(mysql_query(mysql, query)){
		printf("Could not find Users\n");
	}
}

void printUsage() {
	printf("Flag: -clear Usage: removes all posts, users, streams and other information from the tables\n");
	printf("Flag: -reset Usage: deletes the tables from the database\n");
	printf("Flag: -posts Usage: prints out all posts stored in the database\n");
	printf("Flag: -users Usage: prints out all user names stored in the database\n");
	printf("Flag: -streams Usage: prints out all stream names stored in the database\n");
	printf("Flag: -help Usage: prints this message\n");
	printf("----------------------\nCustom Functions:\n----------------------\n");
	printf("Flag: -addauthor Usage: [add|remove] [stream_name] \"[user_name]\"\n");
	printf("Flag: -addpost Usage: [stream_name] \"[Date]\" \"[user_name]\" \"[Message]\" \n");
	printf("All variables must be present, as well as all END tags, for custom functions\n");
}

void clrstr(char *buf) {
	buf[0] = '\0';
}
