#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct userPost {
    char *username;
    char *streamname;
    char *date;
    char *text;
};

void updateStream(struct userPost *st);

void addUser(char *username, char *list);

void removeUser(char *username, char *list);
