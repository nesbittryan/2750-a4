#include "stream.h"

int main(int argc, char ** argv) {
    if(argc < 4) {
        printf("USAGE: ./addauthor [add|remove] \"[username]\" streamList[, seperated, no spaces]\n");
        printf("ie. ./addauthor add \"Ryan Nesbitt\" s1,s2,s3,s4\n");
        return(-1);
    }
    int rFlag = 0;
    if(strcmp(argv[1], "remove") == 0) {
        rFlag = 1;
    }
    char username[64];
    char streams[100];
    strcpy(username, argv[2]);
    strcpy(streams, argv[3]);
    if(rFlag == 1) {
        removeUser(username, streams);
    } else {
        addUser(username, streams);
    }
    return(0);
}
