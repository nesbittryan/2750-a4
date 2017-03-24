#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *removeSpaces(char * str);

void printToFile(char *tok);

void parseLine(char *buffer, char * username, char *streamname, char *messagenum);

void addLink(char *tok);

void addText(char *tok);

void addHeading(char *tok);

void addLine(char *tok);

void addImage(char *tok);

void addButton(char *tok, char *username, char *streamname, char *messagenum);

void addInput(char *tok, char *username, char *streamname, char *messagenum);

void addRadioButton(char *tok, char *username, char *streamname, char *messagenum);

void addExe(char *tok);

int main(int argc, char **argv) {
    if(argc < 5) {
        printf("USAGE: ./sc filename.wpml STREAMNAME MESSAGENUM USERNAME\n");
        printf("(Use STREAM_NULL, MSG_NULL, and NAME_NULL if not applicable)\n");
        printf("ie. ./sc filename.wpml STREAM_NULL MSG_NULL NAME_NULL\n");
        return(-1);
    }
    int i = 1;
    char *filename = malloc(strlen(argv[i]) + 1);
    strcpy(filename, argv[i]);
    ++i;
    char *buffer = malloc(sizeof(char) * 1000);
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Invalid File\n");
        return(-1);
    }
    char username[500];
    char streamname[50];
    char messagenum[50];

    strcpy(streamname, argv[i]);
    ++i;
    strcpy(messagenum, argv[i]);
    ++i;
    strcpy(username, argv[i]);
    ++i;
    while(argv[i] != NULL) {
        strcat(username, " ");
        strcat(username, argv[i]);
        ++i;
    }
    printf("<html>\n");

    while(fgets(buffer, 999,fp)) {
        buffer[strlen(buffer) - 1] = '\0';
        parseLine(buffer, username, streamname, messagenum);
    }

    printf("</html>\n");
    fclose(fp);

    free(buffer);
    free(filename);
    return(0);
}

void parseLine(char *buffer, char * username, char *streamname, char *messagenum) {
    char *tok = strtok(buffer, ")");
    while(tok != NULL) {
        if(tok[0] == '.' && tok[2] == '(') {
            switch(tok[1]) {
                case 'h':
                    addHeading(removeSpaces(tok));
                    break;
                case 't':
                    addText(removeSpaces(tok));
                    break;
                case 'd':
                    addLine(removeSpaces(tok));
                    break;
                case 'l':
                    addLink(removeSpaces(tok));
                    break;
                case 'p':
                    addImage(removeSpaces(tok));
                    break;
                case 'e':
                    addExe(removeSpaces(tok));
                    break;
                case 'b':
                    addButton(removeSpaces(tok), username, streamname, messagenum);
                    break;
                case 'i':
                    addInput(removeSpaces(tok), username, streamname, messagenum);
                    break;
                case 'r':
                    addRadioButton(removeSpaces(tok), username, streamname, messagenum);
                    break;
                default:
                    printToFile(tok);
                    break;
            }
        } else {
            printToFile(tok);
        }
        tok = strtok(NULL, ")");
    }
}

void addExe(char *tok) {
    char * nameptr = strstr(tok, "exe=");
    nameptr += 5;
    char exeName[100];
    int i = 0;
    while(nameptr[i] != '"') {
        exeName[i] = nameptr[i];
        ++i;
    }
    FILE *tempfp;
    if((tempfp = fopen(exeName, "r")) == NULL) {
        char temp[99];
        strcpy(temp, "./bin/");
        strcat(temp, exeName);
        if((tempfp = fopen(temp, "r")) == NULL) {
            printf("<?php\n\texec(\"../../../bin/%s\");\n?>\n", exeName);
        } else {
            fclose(tempfp);
            printf("<?php\n\texec(\"%s\");\n?>\n", exeName);
        }
    } else {
        fclose(tempfp);
        printf("<?php\n\texec(\"./%s\");\n?>\n", exeName);
    }
}

void addRadioButton(char *tok, char *username, char *streamname, char *messagenum) {
    char *action = strstr(tok, "action=");
    char *nameptr, *valueptr;
    int i = 0 , j = 0;
    printf( "<form action=\"");
    action += 8;
    while(action[i] != '"') {
        printf( "%c", action[i]);
        ++i;
    }
    printf( "\" method=\"post\">\n");
    char *hInput = strstr(tok, "hInput=1");
    if(hInput != NULL) {
        if(strcmp(username, "NAME_NULL") != 0) {
            printf("\t<input type=\"hidden\" name=\"username\" value=\"%s\">\n", username);
        }
        if(strcmp(streamname, "STREAM_NULL") != 0) {
            printf("\t<input type=\"hidden\" name=\"streamChoice\" value=\"%s\">\n", streamname);
        }
        if(strcmp(messagenum, "MSG_NULL") != 0) {
            printf("\t<input type=\"hidden\" name=\"messageNum\" value=\"%s\">\n", messagenum);
        }
    }
    nameptr = strstr(tok, "name=");
    nameptr += 6;
    while((valueptr = strstr(tok, "value=")) != NULL) {
        valueptr += 7;
        printf("\t");
        printf("<input type=\"radio\" name=\"");
        i = 0;
        while(nameptr[i] != '"') {
            printf( "%c", nameptr[i]);
            ++i;
        }
        printf("\" value=\"");
        i = 0;
        while(valueptr[i] != '"') {
            printf("%c", valueptr[i]);
            ++i;
        }
        if(j == 0) {
            printf("\" checked>");
        } else {
            printf("\">");
        }
        i = 0;
        while(valueptr[i] != '"') {
            printf("%c", valueptr[i]);
            ++i;
        }
        printf("<br>\n");
        tok = valueptr;
        j = 1;
    }
    printf("\t<input type=\"submit\" value=\"submit\">\n</form>\n");
}

void addInput(char *tok, char *username, char *streamname, char *messagenum) {
    char *action = strstr(tok, "action=");
    char *textptr, *nameptr, *valueptr;
    char *hInput = strstr(tok, "hInput=1");
    int i = 0;
    printf( "<form action=\"");
    action += 8;
    while(action[i] != '"') {
        printf( "%c", action[i]);
        ++i;
    }
    printf("\" method=\"post\">\n");
    if(hInput != NULL) {
        if(strcmp(username, "NAME_NULL") != 0) {
            printf("\t<input type=\"hidden\" name=\"username\" value=\"%s\">\n", username);
        }
        if(strcmp(streamname, "STREAM_NULL") != 0) {
            printf("\t<input type=\"hidden\" name=\"streamChoice\" value=\"%s\">\n", streamname);
        }
        if(strcmp(messagenum, "MSG_NULL") != 0) {
            printf("\t<input type=\"hidden\" name=\"messageNum\" value=\"%s\">\n", messagenum);
        }
    }
    while((textptr = strstr(tok, "text=")) != NULL) {
        textptr += 6;
        nameptr = strstr(textptr, "name=");
        valueptr = strstr(textptr, "value=");
        printf( "\t");
        while(textptr[0] != '"') {
            printf( "%c", textptr[0]);
            ++textptr;
        }
        printf("<input type=\"text\" name=\"");
        nameptr += 6;
        while(nameptr[0] != '"') {
            printf( "%c", nameptr[0]);
            ++nameptr;
        }
        printf("\" value=\"");
        valueptr += 7;
        while(valueptr[0] != '"') {
            printf( "%c", valueptr[0]);
            ++valueptr;
        }
        printf("\">\n");

        tok = textptr;
    }
    printf("\t<input type=\"submit\">\n</form>\n");
}

void addButton(char *tok, char *username, char *streamname, char *messagenum) {
    char *namePtr = strstr(tok, "name="), *linkPtr = strstr(tok, "link=");
    if(namePtr == NULL || linkPtr == NULL) return;
    namePtr +=6;
    linkPtr +=6;
    printf("<form action=\"");
    int i = 0;
    while(linkPtr[i] != '"') {
        printf( "%c", linkPtr[i]);
        ++i;
    }
    char *hInput = strstr(tok, "hInput=1");
    if(hInput != NULL) {
        printf("\" method=\"post\">\n");
        if(strcmp(username, "NAME_NULL") != 0) {
            printf("\t<input type=\"hidden\" name=\"username\" value=\"%s\">\n", username);
        }
        if(strcmp(streamname, "STREAM_NULL") != 0) {
            printf("\t<input type=\"hidden\" name=\"streamChoice\" value=\"%s\">\n", streamname);
        }
        if(strcmp(messagenum, "MSG_NULL") != 0) {
            printf("\t<input type=\"hidden\" name=\"messageNum\" value=\"%s\">\n", messagenum);
        }
    } else {
        printf("\">\n\t");
    }
    printf("<input type=\"submit\" value=\"");
    i =0;
    while(namePtr[i] != '"') {
        printf( "%c", namePtr[i]);
        ++i;
    }
    printf("\"/>\n</form>\n");
}

void addImage(char *tok ) {
    char *sizePtr = strstr(tok, "size="), *imagePtr = strstr(tok, "image=");
    printf("<img src=");
    imagePtr += 6;
    if(sizePtr != NULL) {
        int i = 0;
        while(imagePtr[i] != ',') {
            printf( "%c", imagePtr[i]);
            ++i;
        }
        sizePtr += 5;
        char *sizeTok = strtok(sizePtr, "x");
        printf(" width=\"%s\"", sizeTok);
        sizeTok = strtok(NULL, ")");
        printf(" height =\"%s\"", sizeTok);
    } else {
        printf("%s width=\"100\" height = \"100\"", imagePtr);
    }
    printf(">\n");
}

void addLine(char *tok ) {
    printf("<hr>\n");
}

void addText(char *tok ) {
    char *textptr = strstr(tok, "text="), *filenameptr = strstr(tok, "file=");
    printf("<p>");
    if(filenameptr == NULL) {
        if(textptr == NULL) {
            printf("Default text");
        } else {
            textptr += 6;
            while(textptr[0] != '"') {
                printf("%c", textptr[0]);
                ++textptr;
            }
        }
    } else {
        filenameptr += 6;
        char inFileName[100], c;
        int i = 0;
        while(filenameptr[i] != '"') {
            inFileName[i] = filenameptr[i];
            ++i;
        }
        FILE *infp = fopen(inFileName, "r");
        if(infp == NULL) {
            printf( "Invalid File");
        } else {
            while((c = fgetc(infp)) != EOF) {
                printf( "%c", c);
            }
            fclose(infp);
        }
    }
    printf( "</p>");
}

void addLink(char *tok ) {
    char *linkptr = strstr(tok, "link="), *textptr = strstr(tok, "text=");
    linkptr += 6;
    printf("<a href=\"");
    while(linkptr[0] != '"') {
        printf( "%c", linkptr[0]);
        ++linkptr;
    }
    printf("\">\n");
    if(textptr == NULL) {
        printf("link");
    } else {
        textptr += 6;
        while(textptr[0] != '"') {
            printf( "%c", textptr[0]);
            ++textptr;
        }
    }
    printf( "</a>\n");
}

void addHeading(char *tok ) {
    char *sizeptr = strstr(tok, "size="), *textptr = strstr(tok, "text=");
    if(sizeptr == NULL) {
        printf( "<h3>\n");
        if(textptr == NULL) {
            printf( "HEADING");
        } else {
            textptr += 6;
            while(textptr[0] != '"') {
                printf( "%c", textptr[0]);
                ++textptr;
            }
        }
        printf( "</h3>\n");
    } else {
        sizeptr += 5;
        printf( "<h%c>\n", sizeptr[0]);
        if(textptr == NULL) {
            printf("HEADING");
        } else {
            textptr += 6;
            while(textptr[0] != '"') {
                printf( "%c", textptr[0]);
                ++textptr;
            }
        }
        printf( "</h%c>\n", sizeptr[0]);
    }
}

void printToFile(char *tok ) {
    printf( "%s", tok);
}
char *removeSpaces(char * str) {
    int i;
    for(i = 0; i < strlen(str); ++i) {
        if(str[i] == '"') {
            ++i;
            while(str[i] != '"') {
                ++i;
            }
            ++i;
        }
        if(str[i] == ' ') {
            int j;
            for(j = i; j < strlen(str) - 1; ++j) {
                str[j] = str[j+1];
            }
            str[strlen(str) - 1] = '\0';
            --i;
        }
    }
    return str;
}
