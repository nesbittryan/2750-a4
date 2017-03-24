#include "stream.h"

void updateStream(struct userPost *st) {
    int permission = 0;
    FILE *streamFile = NULL, *userStreamFile = NULL, *streamDataFile = NULL;
    char * streamFilename = malloc(sizeof(char) * strlen(st->streamname) + 20);
    char * userStreamFilename = malloc(sizeof(char) * strlen(st->streamname) + 20);
    char * streamDataFilename = malloc(sizeof(char) * strlen(st->streamname) + 20);
    /* creating stream file name */
    strcpy(streamFilename, "messages/");
    strcat(streamFilename,  st->streamname);
    strcat(streamFilename, "Stream");
    /* creating user stream file name */
    strcpy(userStreamFilename, "messages/");
    strcat(userStreamFilename,  st->streamname);
    strcat(userStreamFilename, "UserStream");
    /* creating stream data file name */
    strcpy(streamDataFilename, "messages/");
    strcat(streamDataFilename,  st->streamname);
    strcat(streamDataFilename, "StreamData");

    streamFile = fopen(streamFilename, "r");
    if(streamFile == NULL) {
        printf("Steam %s does not exist, exiting...\n", st->streamname);
        free(streamFilename);
        free(userStreamFilename);
        free(streamDataFilename);
        return;
    } else {
        /* STREAM DOES EXIST */
        fclose(streamFile);
    }
    /* Checking if user has permission */
    userStreamFile = fopen(userStreamFilename, "r");
    char str[100];
    int lineCount = 0;
    while(fgets(str, 100, userStreamFile) != NULL) {
        ++lineCount;
        char * copyStr = malloc(sizeof(char) * strlen(str) + 1);
        strcpy(copyStr, str);
        char * ptr = copyStr;
        int i = 0;
        for(i = strlen(ptr) - 1; i > 0; --i) {
            if(ptr[i] == ' ') {
                ptr[i] = '\0';
                break;
            }
        }
        if(strcmp(copyStr, st->username) == 0) {
            permission = 1;
        }
        free(copyStr);
    }
    fclose(userStreamFile);
    if(permission == 1) {
        streamFile = fopen(streamFilename, "a+");
        fprintf(streamFile,"Sender: %s\n", st->username);
        fprintf(streamFile,"Date: %s\n", st->date);
        fprintf(streamFile,"%s", st->text);
        fclose(streamFile);

        userStreamFile = fopen(userStreamFilename, "r");
        int flag = 0;
        while(fscanf(userStreamFile, "%s", str) == 1) {
            if(strcmp(str, st->username) == 0) {
                flag = 1;
            }
        }
        fclose(userStreamFile);
        if(flag == 0) {
            userStreamFile = fopen(userStreamFilename, "a");
            fprintf(userStreamFile,"%s 0\n", st->username);
            fclose(userStreamFile);
        }

        char ch;
        int count = 0;
        streamFile = fopen(streamFilename, "r");
        while((ch = getc(streamFile)) != EOF) {
            ++count;
        }
        fclose(streamFile);
        streamDataFile = fopen(streamDataFilename, "a+");   /* FINISH STREAM DATA FILE */
        fprintf(streamDataFile, "%d\n", count);
        fclose(streamDataFile);
        printf("Message posted\n");

    } else {
        printf("%s does not have permission for stream %s...\n", st->username, st->streamname);
    }
    free(streamFilename);
    free(userStreamFilename);
    free(streamDataFilename);
}

void addUser(char *username, char *list) {
    FILE *userStreamFile = NULL;
    char * listCopy = malloc(sizeof(char) * strlen(list) + 1);
    strcpy(listCopy, list);
    char * token = strtok(listCopy, ",");
    int flag = 0;
    /* parsing through list */
    while(token != NULL) {
        char * filename = malloc(sizeof(char) * strlen(list) + 20);
        strcpy(filename, "messages/");
        strcat(filename,  token);
        strcat(filename, "UserStream");
        userStreamFile = fopen(filename, "r");
        if(userStreamFile == NULL) {
            /* STREAM DOES NOT EXIST */
            FILE* streamFile = NULL, *streamDataFile = NULL, *streamList = NULL;
            streamList = fopen("messages/streamList", "a+");
            fprintf(streamList, "%s\n", token);
            fclose(streamList);
            printf("Creating stream %s...\n", token);
            char * streamName = malloc(sizeof(char) * strlen(list) + 20);
            strcpy(streamName, "messages/");
            strcat(streamName,  token);
            strcat(streamName, "Stream");
            /* creating Stream file */
            streamFile = fopen(streamName, "w");
            fclose(streamFile);

            strcat(streamName, "Data");
            /* creating data stream file */
            streamDataFile = fopen(streamName, "w");
            fclose(streamDataFile);
            free(streamName);
        } else {
            /* STREAM DOES EXIST */
            char str[100];
            while(fgets(str, 99, userStreamFile) != NULL) {
                char * copyStr = malloc(sizeof(char) * strlen(str) + 1);
                strcpy(copyStr, str);
                char * ptr = copyStr;
                int i = 0;
                for(i = strlen(ptr) - 1; i > 0; --i) {
                    if(ptr[i] == ' ') {
                        ptr[i] = '\0';
                        break;
                    }
                }
                if(strcmp(copyStr, username) == 0) {
                    printf("%s already has access to %s, aborting...\n", username, token);
                    flag = 1;
                }
                free(copyStr);
            }
            fclose(userStreamFile);
        }
        if(flag == 0) {
            userStreamFile = fopen(filename, "a+");
            /* adding user to end of file */
            fprintf(userStreamFile, "%s 0\n", username);
            fclose(userStreamFile);
            printf("%s added to stream %s...\n", username, token);
        }

        free(filename);
        token = strtok(NULL, ",");
    }
    free(listCopy);
}

void removeUser(char *username, char *list) {
    FILE *userStreamFile = NULL;
    char * listCopy = malloc(sizeof(char) * strlen(list) + 1);
    strcpy(listCopy, list);

    char * token = strtok(listCopy, ",");
    while(token != NULL) {
        char * filename = malloc(sizeof(char) * strlen(list) + 20);
        int flag = 0;
        strcpy(filename, "messages/");
        strcat(filename,  token);
        strcat(filename, "UserStream");

        userStreamFile = fopen(filename, "r+");
        if(userStreamFile != NULL) {
            /* STREAM DOES EXIST */
            FILE *copy = fopen("messages/copy", "w");
            char str[100];
            while(fgets(str, 99, userStreamFile) != NULL) {
                char * copyStr = malloc(sizeof(char) * strlen(str) + 1);
                strcpy(copyStr, str);
                char * ptr = copyStr;
                int i = 0;
                for(i = strlen(ptr) - 1; i > 0; --i) {
                    if(ptr[i] == ' ') {
                        ptr[i] = '\0';
                        break;
                    }
                }
                if(strcmp(copyStr, username) != 0) {
                    fprintf(copy, "%s", str);
                } else {
                    printf("Removing %s\n", username);
                    flag = 1;
                }
                free(copyStr);
            }
            if(flag == 0) {
                printf("%s not found\n",username);
            }
            rename("messages/copy", filename);
            fclose(copy);
            fclose(userStreamFile);
        } else {
            /* STREAM DOES NOT EXIST */
            printf("%s Does not exist...\n", token);
        }
        free(filename);
        token = strtok(NULL, ",");
    }
    free(listCopy);
}
