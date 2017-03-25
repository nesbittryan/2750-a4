/*==========================================================================

Example using mySQL

==========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#define MAX_QUERY 512
#define HOSTNAME  "dursley.socs.uoguelph.ca"
/*
	HOSTNAME can be defined with IP or hostname.

	eg: #define HOSTNAME "131.104.49.147"
	    #define HOSTNAME "gradpc-38.cis.uoguelph.ca"

*/

#define USERNAME  "nesbittr"
#define PASSWORD  "0915819"
#define DATABASE  "database"

/*
	standard error clause.
*/
void error(char *msg, MYSQL *mysql){
	printf("%s\n%s\n",msg,mysql_error(mysql));
	exit(1);
}

/*
	clear the contents of an array of char's
*/
void clrstr(char *buf){
	buf[0] = '\0';
}

/*==========================================================================
	MAIN - mydb.c
==========================================================================*/
int main(int argc, char *argv[]) {
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	char query[MAX_QUERY];
	int x;
	int i = 0;
	char *records[] = {
		"insert into students values (1,'Hugo','Victor','B+')",
		"insert into students values (4,'Rudin','Walter','A-')",
		"insert into students values (5,'Stevens','Richard','C')" };

	/*
		Connect to database server.
		Username and password must be filled in here.
		If no username or password is stored in DB then use NULL.
	*/
	printf("connecting...\n");

	mysql_init(&mysql);
	mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&mysql, HOSTNAME, USERNAME, PASSWORD,
		DATABASE, 0, NULL, 0)) {
	   error("Could not connect to host.",&mysql);
	}

	printf("Selecting a specific database to work with.\n");

	/*
		Build query
	*/

	clrstr(query);

	strcat(query, "create table students (id int not null auto_increment,");
	strcat(query, "last_name char(15),");
	strcat(query, "first_name char(15),");
	strcat(query, "mark char(2),");
	strcat(query, "primary key(id) )");

	printf("Creating students table.\n");

	/*
		Create students table
	*/
	if(mysql_query(&mysql, query)){
	  error("Could not create table!",&mysql);
	}

	clrstr(query);

	/*
		Insert a records into the students table
	*/

	printf("Inserting students.\n");

	for(x = 0; x < 3; x++){
		if(mysql_query(&mysql, records[x])){
			printf("Failure to insert: %s\n",records[x]);
			error("Could not insert record",&mysql);
		}
	}

	clrstr(query);

	/*
		Let us look at what we inserted
	*/
	strcpy(query, "select * from students order by last_name");

	if(mysql_query(&mysql, query))
	  error("failed select 1",&mysql);

	/*
		Store results from query into res structure.
	*/
	if (!(res = mysql_store_result(&mysql))){
		error("failed store 1",&mysql);
	}

	/*
		print first row elements
	*/
	while ((row = mysql_fetch_row(res))) {
		printf("%s ", row[0]);
	}

	printf("\n");

	/*
		second query
	*/
	strcpy(query, "select * from students order by last_name");

	if(mysql_query(&mysql, query)){
		error("fail select 2",&mysql);
	}

	/*
		Store results from query into res structure.
	*/
	if (!(res = mysql_store_result(&mysql))){
		error("fail store 2",&mysql);
	}

	/*
		print all results
	*/
	while ((row = mysql_fetch_row(res))) {
		for (i=0; i < mysql_num_fields(res); i++){
			printf("%s ", row[i]);
		}
		printf("\n");
	}

	/*
		Drop the students table
	*/

	strcpy(query, "drop table students");

	if(mysql_query(&mysql,query))
	  error("fail drop 1",&mysql);

	/*
		Finally close connection to server
	*/
	mysql_close(&mysql);

	printf("All done\n");

	return 0;
}
