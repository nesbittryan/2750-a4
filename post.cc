#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include"stream.h"

class PostEntry {

    void formatEntry(struct userPost *up, char * name, char * stream, char * text, char * date) {
        up->username = (char*)malloc(sizeof(char) * strlen(name) + 1);
        up->streamname = (char*)malloc(sizeof(char) * strlen(stream) + 1);
        up->text = (char*)malloc(sizeof(char) * strlen(text) + 1);
        up->date = (char*)malloc(sizeof(char) * strlen(date) + 1);
        strcpy(up->username, name);
        int i;
        for(i = strlen(stream); i >= 0; --i) {
            if(stream[i] == ' ' || stream[i] == '\n') {
                stream[i] = '\0';
            }
        }
        strcpy(up->streamname, stream);
        strcpy(up->text, text);
        strcpy(up->date, date);
    }

    void getTimeDate(char * date) {
        char copy[100];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        switch(tm.tm_mon + 1) {
            case 1:
                strcpy(date, "Jan. ");
                break;
            case 2:
                strcpy(date, "Feb. ");
                break;
            case 3:
                strcpy(date, "Mar. ");
                break;
            case 4:
                strcpy(date, "Apr. ");
                break;
            case 5:
                strcpy(date, "May. ");
                break;
            case 6:
                strcpy(date, "Jun. ");
                break;
            case 7:
                strcpy(date, "Jul. ");
                break;
            case 8:
                strcpy(date, "Aug. ");
                break;
            case 9:
                strcpy(date, "Sep. ");
                break;
            case 10:
                strcpy(date, "Oct. ");
                break;
            case 11:
                strcpy(date, "Nov. ");
                break;
            case 12:
                strcpy(date, "Dec. ");
                break;
        }

        sprintf(copy, "%d,", tm.tm_mday);
        strcat(date, copy);
        strcat(date, " ");
        sprintf(copy, "%d,", tm.tm_year + 1900);
        strcat(date, copy);
        if(tm.tm_hour < 12) {
            sprintf(copy, " %d:%02d", tm.tm_hour, tm.tm_min);
            strcat(date, copy);
            strcat(date, " AM");
        } else {
            if(tm.tm_hour == 12) {
                sprintf(copy, " %d:%02d", tm.tm_hour, tm.tm_min);
            } else {
                sprintf(copy, " %d:%02d", tm.tm_hour - 12, tm.tm_min);
            }
            strcat(date, copy);
            strcat(date, " PM");
        }
    }
    void submitPost(struct userPost sp) {
        updateStream(&sp);
        free(sp.username);
        free(sp.streamname);
        free(sp.date);
        free(sp.text);
        return;
    }
};

int main(int argc, char ** argv) {

    if(argc < 5) {
        printf("USAGE: ./post streamname username[multiple words] MESSAGE_TEXT Message[multiple words]\n");
        printf("ie. ./post s1 Ryan Nesbitt MESSAGE_TEXT Hello this is my message\n");
        return(-1);
    }
    int i = 3;
    char name[100], stream[100], text[1000], date[100];
    strcpy(stream, argv[1]);
    strcpy(name, argv[2]);
    while(strcmp(argv[i],"MESSAGE_TEXT") != 0) {
        strcat(name, " ");
        strcat(name, argv[i]);
        ++i;
    }
    ++i;
    strcpy(text, argv[i]);
    ++i;
    while(argv[i] != NULL) {
        strcat(text, " ");
        strcat(text, argv[i]);
        ++i;
    }
    strcat(text, "\n");
    struct userPost up;
    class PostEntry myPost;
    myPost.getTimeDate(&date[0]);
    myPost.formatEntry(&up, name, stream, text, date);
    myPost.submitPost(up);
    return (0);
}
