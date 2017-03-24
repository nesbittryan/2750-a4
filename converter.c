#include"converter.h"

int main(int argc, char ** argv) {

    char *inFileName = malloc(sizeof(char) * 100), *outFileName =  malloc(sizeof(char) * 100);
    strcpy(inFileName,argv[1]);
    strcpy(outFileName,inFileName);
    outFileName[strlen(outFileName)-1] = '\0';

    Head * head = initList(inFileName);
    if(head == NULL) {
        free(inFileName);
        free(outFileName);
        return(-1);
    }
    convertToC(head);

    finishStructs(head);

    finishFunc(head);

    addConstructors(head->firstObject->next);

    char ** funcList = getFuncList(head->firstObject->next);
    char ** objList = getStructObjList(head->firstObject->next);

    finishMain(head->firstObject->next, funcList, objList);

    finishFuncPtrs(head->firstObject->next);

    int s;
    for(s = 0; s < 50; ++s) {
        free(funcList[s]);
        free(objList[s]);
    }
    free(funcList);
    free(objList);
    printToFile(head->firstObject->next, outFileName);
    free(head->firstObject->value);
    free(head->firstObject);
    free(head);
    free(inFileName);
    free(outFileName);
    return(0);
}

Head *initList(char * inFile) {
    Head *head = malloc(sizeof(Head));
    head->firstObject = createObject("//FILESTART");
    FILE *infp = fopen(inFile, "r");
    if(infp) {
        int comment = 0;
        char ch = ' ', *buffer = malloc(sizeof(char) * 1000), prevch = ' ';
        clearBuffer(buffer);

        while((ch = getc(infp)) != EOF) {
            if(prevch == '/' && ch == '/') {
                comment = 1;
            }
            if(prevch == '/' && ch == '*') {
                comment = 2;
            } else if(prevch == '/' && comment == 0) {
                addObject(head->firstObject, buffer);
                memset(buffer,0,strlen(buffer));
            }
            if(comment == 2) {
                addToBuffer(buffer, ch);
                if(prevch == '*' && ch == '/') {
                    addObject(head->firstObject, buffer);
                    memset(buffer,0,strlen(buffer));
                    comment = 0;
                }
            } else {
            switch(ch) {
                case '\n':
                    if(comment == 1) {
                        addToBuffer(buffer, ch);
                        comment = 0;
                    }
                    if(buffer[0] != '\0') {
                        addObject(head->firstObject, buffer);
                        memset(buffer,0,strlen(buffer));
                    }
                    break;
                case '{':
                case '}':
                case '(':
                case ')':
                case ';':
                case ',':
                case ':':
                case '+':
                case '-':
                case '*':
                    if(buffer[0] != '\0') {
                        addObject(head->firstObject, buffer);
                        memset(buffer,0,strlen(buffer));
                    }
                    addToBuffer(buffer, ch);
                    if(buffer[0] != '\0') {
                        addObject(head->firstObject, buffer);
                        memset(buffer,0,strlen(buffer));
                    }
                    break;
                case ' ':
                case '\t':
                case '\r':
                    if(buffer[0] != '\0') {
                        addObject(head->firstObject, buffer);
                        memset(buffer,0,strlen(buffer));
                    }
                    break;
                default:
                    addToBuffer(buffer, ch);
                    break;
            }}
            prevch = ch;
        }
        fclose(infp);
        free(buffer);
        return (head);
    } else {
        printf("File Does Not Exist\n");
        free(head->firstObject->value);
        free(head->firstObject);
        free(head);
        return (NULL);
    }
}

List *createObject(char * value) {
    List *newObject = NULL;
    if((newObject = malloc(sizeof(List))) != NULL) {
        if((newObject->value = malloc(sizeof(char) * 500)) != NULL) {
            strcpy(newObject->value, value);
            newObject->next = NULL;
            return (newObject);
        } else {
            free(newObject);
        }
    }
     return (NULL);
}

void clearBuffer(char * buffer) {
    int i;
    for(i = 0; i < 100; ++i) {
        buffer[i] = '\0';
    }
}

char *addToBuffer(char * buffer, char ch) {
    buffer[strlen(buffer)] = ch;
    return buffer;
}

void addObject(List * object, char * value) {
    if(object->next != NULL) {
        addObject(object->next, value);
    } else {
        List *newObject = createObject(value);
        object->next = newObject;
    }
}

Head * convertToC(Head * head) {
    List *funcList = createObject("\n"), *afterStructs = NULL, *current = head->firstObject->next, *constCheck = NULL;
    while(current->next != NULL) {
        if((strcmp(current->value, "}") == 0) && (strcmp(current->next->value, ";") == 0)) {
            afterStructs = current->next;
        }
        current = current->next;
    }
    current = head->firstObject->next;
    while(current != NULL) {
        if(strstr(current->value, "class") != NULL) {
            constCheck = current;
            classToStruct(current);
            if(strstr(current->next->next->value, "{")) {
                char *structName = malloc(sizeof(char) * (strlen(current->next->value) + 1));
                strcpy(structName, current->next->value);
                convertStructs(structName, current->next->next);
                removeFuncFromStructs(funcList, current->next->next->next);
                if(needConstructor(constCheck) == 1) {
                    addToEnd(funcList, constructorCreator(structName, current));
                }
                free(structName);
            }
        }
        current = current->next;
    }
    if(funcList->next != NULL) {
        addToEnd(funcList->next, afterStructs->next);
        afterStructs->next= funcList->next;
    }
    free(funcList->value);
    free(funcList);
    return (head);
}

void convertStructs(char * structName, List *object) {
    List *current = object, *before = current;
    int structFound = 0;
    char *newVal = malloc(sizeof(char) * 200);
    clearBuffer(newVal);
    while(!((strcmp(current->value, "}") == 0) && (strcmp(current->next->value,";")) == 0)) {
        if(isKeyword(current->value) != 0) {
            List *nextChar = current;
            while(nextChar != NULL) {
                if(strstr(nextChar->value, ";")) {
                    break;
                }
                if(strcmp(nextChar->value, "{") == 0) {
                    if(isKeyword(current->value) == 2) {
                        ++structFound;
                    }
                    funcPrototype(newVal, structName, current, structFound);
                    List *newObject = createObject(newVal);
                    clearBuffer(newVal);
                    if(structFound != 0) {
                        changeName(newVal, structName, current->next->next);
                        strcpy(current->next->next->value, newVal);
                        strcpy(current->next->next->next->value, "\0");
                    } else {
                        changeName(newVal, structName, current->next);
                        strcpy(current->next->value, newVal);
                    }
                    List * new = createObject(" ");
                    newObject->next = current;
                    before->next = new;
                    new->next = newObject;
                }
                nextChar = nextChar->next;
            }
            current = nextChar;
        }
        if(structFound != 0 && ((strcmp(current->next->value, "}") == 0) && (strcmp(current->next->next->value,";")) == 0)) {
            --structFound;
            current = current->next->next;
        }
        before = current;
        current = current->next;
    }
    free(newVal);
}

void funcPrototype(char * newVal, char *structName, List *current, int structFound) {
    if(structFound != 0) {
        strcpy(newVal, current->value);
        strcat(newVal, " ");
        current = current->next;
        strcat(newVal, current->value);

    } else {
        strcpy(newVal, current->value);
    }
    if(strcmp(current->next->value, "*") == 0) {
        current = current->next;
        strcat(newVal, " ");
        strcat(newVal, current->value);
    }
    strcat(newVal, " (*");
    current = current->next;
    strcat(newVal, structName);
    strcat(newVal, current->value);
    current = current->next;

    while(!(strstr(current->next->value, "{"))) {
        if(strcmp(current->value, "int") == 0) {
            strcat(newVal, "i");
        } else if(strcmp(current->value, "char") == 0) {
            strcat(newVal, "c");
        } else if(strcmp(current->value, "double") == 0) {
            strcat(newVal, "d");
        } else if(strcmp(current->value, "long") == 0) {
            strcat(newVal, "l");
        } else if(strcmp(current->value, "float") == 0) {
            strcat(newVal, "f");
        } else if(strcmp(current->value, "short") == 0) {
            strcat(newVal, "s");
        }else if(strcmp(current->value, "void") == 0) {
            strcat(newVal, "v");
        }
        current = current->next;
    }
    strcat(newVal, ")();\0");
}

void changeName(char * toChange, char *structName, List * current) {
    if(strcmp(current->value, "*") == 0) {
        strcpy(toChange, current->value);
        current = current->next;
    }
    strcat(toChange, structName);
    strcat(toChange, current->value);
    current = current->next;
    while(!(strstr(current->next->value, "{"))) {
        if(strcmp(current->value, "int") == 0) {
            strcat(toChange, "i");
        } else if(strcmp(current->value, "char") == 0) {
            strcat(toChange, "c");
        } else if(strcmp(current->value, "double") == 0) {
            strcat(toChange, "d");
        } else if(strcmp(current->value, "long") == 0) {
            strcat(toChange, "l");
        } else if(strcmp(current->value, "float") == 0) {
            strcat(toChange, "f");
        } else if(strcmp(current->value, "short") == 0) {
            strcat(toChange, "s");
        }else if(strcmp(current->value, "void") == 0) {
            strcat(toChange, "v");
        }
        current = current->next;
    }
}

void removeFuncFromStructs(List * funcList, List *current) {
    List *beforeCurrent = current;
    while(!((strcmp(current->value, "}") == 0) && (strcmp(current->next->value, ";") == 0))) {
        if(strcmp(current->next->value, "main") == 0) {
            break;
        }
        if(isKeyword(current->value) != 0) {
            List *nextChar = current;
            int flag = 0;
            while(nextChar != NULL) {
                if(strcmp(nextChar->value, "{") == 0) {
                    addToEnd(funcList, current);
                    removeFromList(beforeCurrent, current);
                    current = beforeCurrent;
                    flag = 1;
                    break;
                }
                if(strcmp(nextChar->value, ";") == 0) {
                    flag = 0;
                    break;
                }
                nextChar = nextChar->next;
            }
            if(flag == 0) {
                current = nextChar;
            } else {
                flag = 0;
            }
        }
        beforeCurrent = current;
        current = current->next;
    }
}

void removeFromList(List *beforeCurrent, List *current) {
    int level = 0;
    while(strcmp(current->value, "{") != 0) {
        current = current->next;
    }
    current = current->next;
    while(level >= 0) {
        if(strcmp(current->value, "{") == 0) {
            ++level;
        }
        if(strcmp(current->value, "}") == 0) {
            --level;
        }
        if(level == -1) {
            break;
        }
        current = current->next;
    }
    beforeCurrent->next = current->next;
    current->next = NULL;
}

List *constructorCreator(char *structName, List *current) {
    char * value = malloc(sizeof(char) * 1000);
    strcpy(value, "void constructor");
    strcat(value, structName);
    strcat(value, " ( struct ");
    strcat(value, structName);
    strcat(value, " *");
    strcat(value, structName);
    strcat(value, "var ) {\n");
    char * copy = malloc(sizeof(char) * 1000);
    while(current != NULL) {
        if((strcmp(current->value, "}") == 0) && (strcmp(current->next->value, ";") == 0)) {
            break;
        } else if(strstr(current->value, ")();")) {
            strcpy(copy, current->value);
            strcat(value, "\t");
            strcat(value, structName);
            strcat(value, "var->");
            char * ptr = copy;
            while(ptr[0] != '(') {
                ++ptr;
            }
            ptr += 2;
            ptr[strlen(ptr) - 4] = '\0';
            strcat(value, ptr);
            strcat(value, " = ");
            strcat(value, ptr);
            strcat(value, ";\n");
        }
        if((isKeyword(current->value) != 0) && (strcmp(current->next->next->value, "=") == 0)){
            strcat(value, "\t");
            strcat(value, structName);
            strcat(value, "var->");
            strcat(value, current->next->value);
            strcat(value, " = ");
            strcat(value, current->next->next->next->value);
            strcat(value, ";\n");
        }

        current = current->next;
    }
    strcat(value, "}\n");
    List *constructorObject = createObject(value);
    free(copy);
    free(value);
    return constructorObject;
}

int needConstructor(List * current) {
    while(!((strcmp(current->value, "}") == 0) && (strcmp(current->next->value, ";") == 0))) {
        if((strstr(current->value, ")();")) || (strstr(current->value, "="))) {
            return (1);
        }
        current = current->next;
    }
    return(0);
}

List * skipComment(List * current) {
    if(strstr(current->value, "/*")) {
        while(!(strstr(current->value, "*/"))) {
                current = current->next;
            }
        current = current->next;
    }
    if(strstr(current->value, "//")) {
        while(!(strstr(current->value, "\n"))) {
                current = current->next;
            }
        current = current->next;
    }
    return current;
}

List * skipQuotes(List * current) {
    int count = 0;
    int i;
    for(i = 0; i < strlen(current->value); ++i) {
        if(current->value[i] == '"') {
            ++count;
        }
    }
    if(count == 2) {

        return (current->next);
    }
    current = current->next;
    while(!(strstr(current->value, "\""))) {
        current = current->next;
    }
    current = current->next;
    return current;
}

void addToEnd(List *list, List *toAdd) {
    if(list->next != NULL) {
        addToEnd(list->next, toAdd);
    } else {
        list->next = toAdd;
    }
}

void finishStructs(Head * head) {
    List *current = head->firstObject->next;
    while(current != NULL) {
        if((strcmp(current->value, "struct") == 0) && (strcmp(current->next->next->value, "{") == 0)){
            List * inStruct = current;
            while(!((strcmp(inStruct->value, "}") == 0) && (strcmp(inStruct->next->value, ";") == 0))) {
                inStruct = inStruct->next;
                if(strcmp(inStruct->value, "=") == 0) {
                    strcpy(inStruct->value, "\0");
                    strcpy(inStruct->next->value, "\0");
                }
            }
        }
        current= current->next;
    }
}

void finishFunc(Head * head) {
    List *current = head->firstObject->next;
    char ***structMembers = findMembers(current);
    while(current != NULL) {
        int flag = 0;
        if(strcmp(current->value, "main") == 0) {
            current = current->next->next;
            break;
        }
        if(isKeyword(current->value) != 0) {

            if(strcmp(current->next->value, "main") == 0) {
                break;
            }
            flag = findFlag(current);
            if(flag == 1) {
                char **localList = findLocals(current);
                char * funcName = malloc(sizeof(char) * 100);
                if(isKeyword(current->value) == 2) {
                    current = current->next;
                }
                strcpy(funcName, current->next->value);
                List * inFunction = current;
                char ** paramList;
                paramList = findParameters(current);
                while(!(strcmp(inFunction->value, "}") == 0)) {
                    inFunction = inFunction->next;
                    int structFoundFlag = -1;
                    int l;
                    for(l = 0; l < 20; ++l) {
                        if((structMembers[l][0] != NULL) && (strstr(funcName, structMembers[l][0]))) {
                            structFoundFlag = l;
                            break;
                        }
                    }
                    if(structFoundFlag != -1) {
                        findReplaceVariables(localList, structMembers[structFoundFlag], paramList, current);
                        break;
                    }
                }
                int j;
                for(j = 0; j < 20; ++j) {
                    free(paramList[j]);
                    free(localList[j]);
                }
                free(localList);
                free(paramList);
                free(funcName);
            }
        }
        current = current->next;
    }
    int s, i;
    for(s = 0; s < 20; ++s) {
        for(i = 0; i < 20; ++i) {
            free(structMembers[s][i]);
        }
        free(structMembers[s]);
    }
    free(structMembers);
}

char ***findMembers(List *current) {
    char ***list =malloc(sizeof(char**) * 20);
    int i, k;
    for(i = 0; i < 20; ++i) {
        list[i] = malloc(sizeof(char*) * 20);
        for(k = 0; k < 20; ++k) {
            list[i][k] = malloc(sizeof(char) * 20);
            strcpy(list[i][k], "\0");
        }
    }
    int j = 0, s = 0;
    while(current != NULL) {
        if((strcmp(current->value, "struct") == 0) && (strcmp(current->next->next->value, "{") == 0)) {
            j = 0;
            strcpy(list[s][j], current->next->value);
            ++j;
            current = current->next->next->next;
            while(!(strcmp(current->value, "}") == 0)) {
                if(isKeyword(current->value) != 0) {
                    List * inLine = current;
                    while(!(strcmp(inLine->value, ";") == 0)) {
                        if(!(strcmp(inLine->value, ",") == 0) && (isKeyword(inLine->value) == 0) && !(strcmp(inLine->value, "") == 0)){
                            strcpy(list[s][j], inLine->value);
                            ++j;
                        }
                        inLine = inLine->next;
                    }
                }
                current = current->next;
            }
            ++s;
        }
        current = current->next;
    }
    return (list);
}

void classToStruct(List *object) {
    object->value = (char *)realloc(object->value, 7);
    strcpy(object->value, "struct\0");
}

char **findLocals(List *current) {
    char **list = malloc(sizeof(char*) * 20);
    int i;
    for(i = 0; i < 20; ++i) {
        list[i] = malloc(sizeof(char) * 20);
        strcpy(list[i], "\0");
    }
    while(!(strcmp(current->value, "{") == 0)) {
        current = current->next;
    }
    current = current->next;
    int level = 1, s = 0;
    while(level != 0) {
        if(strcmp(current->value, "{") == 0) {
            ++level;
        }
        if(strcmp(current->value, "}")== 0) {
            --level;
        }
        if(isKeyword(current->value) == 1) {
            strcpy(list[s], current->next->value);
            ++s;
        } else if(isKeyword(current->value) == 2) {
            strcpy(list[s], current->next->next->value);
            ++s;
        }
        current = current->next;
    }
    return(list);
}

char **findParameters(List *current) {
    char **list = malloc(sizeof(char*) * 20);
    int i;
    for(i = 0; i < 20; ++i) {
        list[i] = malloc(sizeof(char) * 20);
        strcpy(list[i], "\0");
    }
    int j = 0;
    while(!(strcmp(current->value, "(") == 0)) {
        current = current->next;
    }
    while(!(strcmp(current->value, ")") == 0)) {
        if(isKeyword(current->value) == 1) {
            strcpy(list[j], current->next->value);
            ++j;
        } else if(isKeyword(current->value) == 2) {
            strcpy(list[j], current->next->next->value);
            ++j;
        }
        current = current->next;
    }
    return (list);
}

void findReplaceVariables(char ** localList, char ** structMembers, char ** paramList, List *current) {
    List* beforeVariable = current;
    while(!(strcmp(beforeVariable->value, "(") == 0)) {
        beforeVariable = beforeVariable->next;
    }
    int flag, level = 0, prototypeflag = 0;
    while(!(strcmp(current->value, "{") == 0)) {
            current = current->next;
    }
    current = current->next;
    while(level != -1) {
        if(strcmp(current->value, "class") == 0) {
            classToStruct(current);
        }
        flag = 0;
        if(strcmp(current->value, "}") == 0) {
            --level;
        }
        if(strcmp(current->value, "{") == 0) {
            ++level;
        }
        int i;
        for(i = 0; i < 20; ++i) {
            if(paramList[i] != NULL) {
                if(strcmp(current->value, paramList[i]) == 0) {
                    flag = 1;
                    break;
                }
            }
        }
        for(i = 0; i < 20; ++i) {
            if(localList[i] != NULL) {
                if(strcmp(current->value,  localList[i]) == 0) {
                    flag = 1;
                    break;
                }
            }
        }
        if(flag == 0) {
            for(i = 1; i < 20; ++i) {
                if(structMembers[i] != NULL) {
                    if(strcmp(current->value, structMembers[i]) == 0) {
                        strcpy(current->value, structMembers[0]);
                        strcat(current->value, "var->");
                        strcat(current->value, structMembers[i]);
                        prototypeflag = 1;
                        break;
                    }
                }
            }
        }
        current = current->next;
    }
    if(prototypeflag == 1) {
        char * newString = malloc(sizeof(char) * 100);
        strcpy(newString, "struct ");
        strcat(newString, structMembers[0]);
        strcat(newString, " *");
        strcat(newString, structMembers[0]);
        if(strcmp(beforeVariable->next->value, ")") == 0) {
            strcat(newString, "var\0");
        } else {
            strcat(newString, "var,\0");
        }
        List * newObject = createObject(newString);
        free(newString);
        newObject->next = beforeVariable->next;
        beforeVariable->next = newObject;
    }

}

void addConstructors(List *current) {
    char ** list;
    list = getConstructors(current);
    List *toAddAfter = current;
    int flag = 0, i;
    while(!(strcmp(toAddAfter->value, "main") == 0)) {
        toAddAfter = toAddAfter->next;
    }
    while(!(strcmp(toAddAfter->value, "{") == 0)) {
        toAddAfter = toAddAfter->next;
    }
    while(current->next->next != NULL) {
        if(strcmp(current->value, "main") == 0) {
            flag = 1;
        }
        if((strcmp(current->value, "struct") == 0) && (!(strcmp(current->next->next->value, "{") == 0))) {
            current = current->next;
            for(i = 0; i < 20; ++i) {
                if((list[i] != NULL) && (strcmp(current->value, list[i]) == 0)) {
                    char * structName = malloc(sizeof(char) * 20);
                    strcpy(structName, current->value);
                    current = current->next;
                    if(flag == 1) {
                        toAddAfter = current;
                    }
                    while(!(strcmp(toAddAfter->value, ";") == 0)) {
                        toAddAfter = toAddAfter->next;
                    }
                    while(!(strcmp(current->value, ";") == 0)) {
                        if(!(strcmp(current->value, ",") == 0)) {
                            insertConstructors(toAddAfter, current->value, structName);
                        }
                        current = current->next;
                    }
                    free(structName);
                }
            }
        }
        current = current->next;
    }
    for(i = 0; i < 20; ++i) {
        free(list[i]);
    }
    free(list);
}

char **getConstructors(List *current) {
    char ** list = malloc(sizeof(char*) * 20);
    int s = 0, i;
    for(i = 0; i < 20; ++i) {
        list[i] = malloc(sizeof(char) * 50);
        strcpy(list[i], "\0");
    }
    while(current != NULL) {
        if(strstr(current->value, "constructor")) {
            char * ptr = strstr(current->value, "constructor");
            char copy [1000], *copyPtr = copy;
            strcpy(copy, ptr);
            int i = 0;
            while(copy[i] != ' ') {
                ++i;
            }
            copy[i] = '\0';
            copyPtr += 11;
            strcpy(list[s], copyPtr);
            ++s;
        }
        current = current->next;
    }
    return(list);
}

void insertConstructors(List *toAddAfter, char *itemName, char *structName) {
    char * newString = malloc(sizeof(char) * 100);
    strcpy(newString, "constructor");
    strcat(newString, structName);
    strcat(newString, " ( &");
    strcat(newString, itemName);
    strcat(newString, " );");
    List * newObject = createObject(newString);
    newObject->next = toAddAfter->next;
    toAddAfter->next = newObject;
    free(newString);
}

char **getFuncList(List *current) {
    char ** newList =  malloc(sizeof(char*) * 50);
    int i, s = 0;
    for(i = 0; i < 50; ++i) {
        newList[i] =  malloc(sizeof(char) * 50);
        strcpy(newList[i], "\0");
    }
    while(current != NULL) {
        int flag = 0;
        if(strcmp(current->next->value, "main") == 0) {
            current = current->next->next;
            break;
        }
        if(isKeyword(current->value) != 0) {
            flag = findFlag(current);
            if(flag == 1) {
                if(isKeyword(current->value) == 2) {
                    current = current->next;
                }
                strcpy(newList[s], current->next->value);
                ++s;
            }
        }
        current = current->next;
    }
    return(newList);
}

int findFlag(List * current) {
    int flag1 = 0, flag2 = 0;
    while(!(strcmp(current->value, "}") == 0)) {
        if((strcmp(current->value, "(") == 0) && flag2 == 0 && flag2 == 0) {
            flag1 = 1;
        }
        if((strcmp(current->value, ")") == 0) && flag1 == 1 && flag2 == 0) {
            flag2 = 1;
        }
        if((strcmp(current->value, "{") == 0) && flag2 == 1 && flag2 == 1) {
            return(1);
            break;
        }
        if((strcmp(current->value, ";") == 0)) {
            return(0);
        }
        current = current->next;
    }
    return (0);
}

char **getStructObjList(List * current) {
    char ** newList = malloc(sizeof(char*) * 50);
    int i;
    for(i = 0; i < 50; ++i) {
        newList[i] = malloc(sizeof(char) * 50);
        strcpy(newList[i], "\0");
    }
    while(!(strcmp(current->value, "main") == 0)) {
        current = current->next;
    }
    while(!(strcmp(current->value, "{") == 0)) {
        current = current->next;
    }
    while(!(strstr(current->value, "constructor"))) {
        current = current->next;
    }
    int s = 0;
    while(current != NULL) {
        if(strstr(current->value, "constructor")) {
            char *ptr, * copy = malloc(sizeof(char) * 50);
            ptr = copy;
            strcpy(copy, current->value);
            ptr += 11;
            int j;
            for(j = 0; j < strlen(copy); ++j) {
                if(copy[j] == ')') {
                    copy[j] = '\0';
                }
            }
            strcpy(newList[s], ptr);
            ++s;
            free(copy);
        }
        current = current->next;
    }
    return(newList);
}

void finishMain(List *current, char **funcList, char **objList) {
    List *first = current;
    char **structList =malloc(sizeof(char*) * 50);
    char **objListCopy = malloc(sizeof(char*) * 50);
    int listCount = 0;
    int i;
    for(i = 0; i < 50; ++i) {
        if(strcmp(objList[i], "") != 0) {
            char *objCopy = malloc(sizeof(char) * 100);
            char *strList = malloc(sizeof(char) * 100);
            char *ptr = strstr(objList[i], "&");
            if(ptr != NULL) {
                ++listCount;
                ++ptr;
                strcpy(objCopy, ptr);
                strcpy(strList, objList[i]);
                int j = 0;
                while(strList[j] != ' ') {
                    ++j;
                }
                strList[j] = '\0';
                objCopy[strlen(objCopy) - 1] = '\0';
                structList[i] = strList;
                objListCopy[i] = objCopy;
            }
        }
    }
    while(!(strcmp(current->value, "main") == 0)) {
        current = current->next;
    }
    while(!(strcmp(current->value, "{") == 0)) {
        current = current->next;
    }
    while(current != NULL) {
        List *copyFirst = first;
        int i;
        for(i = 0; i < listCount; ++i) {
            if((strcmp(objListCopy[i], "") != 0) && (strstr(current->value, objListCopy[i]))
                                        && (strcmp(current->next->value, "(") == 0)) {
                char * copy = malloc(sizeof(char) * 100);
                strcpy(copy, objListCopy[i]);
                strcat(copy, ".");
                strcat(copy, structList[i]);
                char * ptr = strstr(current->value, ".");
                ++ptr;
                strcat(copy, ptr);
                List * copyCurrent = current;
                while(!(strcmp(copyCurrent->value, "(") == 0)) {
                    copyCurrent = copyCurrent->next;
                }
                copyCurrent = copyCurrent->next;
                int argFound = 0;
                while(!(strcmp(copyCurrent->value, ")") == 0)) {
                    if(!(strcmp(copyCurrent->value, ",") == 0)) {
                        ++argFound;
                    }
                    copyCurrent = copyCurrent->next;
                }
                char * ptr2 = copy;
                while(ptr2[0] != '.') {
                    ++ptr2;
                }
                ++ptr2;
                while(copyFirst != NULL) {
                    if((strstr(copyFirst->value, ptr2))) {
                        if((strcmp(copyFirst->next->value, "(") == 0) || (strcmp(copyFirst->next->next->value, "(") == 0)) {
                            List *argSearch = copyFirst;
                            int funcArgCount = 0;
                                while(!(strcmp(argSearch->value, ")") == 0)) {
                                if(isKeyword(argSearch->value) != 0) {
                                    ++funcArgCount;
                                }
                                argSearch = argSearch->next;
                            }
                            if(funcArgCount == argFound) {
                                int l;
                                for(l = 0; l < strlen(copy); ++l) {
                                    if(copy[l] == '.') {
                                        int j;
                                        for(j = l +1; j < strlen(copy); ++j) {
                                            copy[j] = '\0';
                                        }
                                    }
                                }
                                strcat(copy, copyFirst->value);
                                int z;
                                for(z = 0; z < strlen(copy); ++z) {
                                    if(copy[z] == '*') {
                                        copy[z] = copy[z + 1];
                                        copy[z + 1] = '*';
                                    }
                                }
                                strcpy(current->value, copy);
                                break;
                            }
                        }
                    }
                    copyFirst = copyFirst->next;
                }
                free(copy);
            }
        }
        current = current->next;
    }
    for(i = 0; i < listCount; ++i) {
        free(objListCopy[i]);
        free(structList[i]);
    }
    free(objListCopy);
    free(structList);
}

int isKeyword(char * value) {
    if(strcmp(value, "int") == 0 || strcmp(value, "char") == 0
    || strcmp(value, "void") == 0 || strcmp(value, "float") == 0
    || strcmp(value, "float") == 0 || strcmp(value, "double") == 0
    || strcmp(value, "int") == 0 || strcmp(value, "long") == 0
    || strcmp(value, "short") == 0) {
        return(1);
    }
    if(strcmp(value, "struct") == 0) {
        return(2);
    }
    return(0);
}

void finishFuncPtrs(List *current) {
    while(current->next != NULL) {
        if(strstr(current->value, ")();")) {
            List *lookAhead = current->next;
            while(strcmp(lookAhead->value, "main") != 0) {
                char copy[200], *ptr = copy;
                strcpy(copy, current->value);
                while(ptr[0] != '(') {
                    ++ptr;
                }
                while(ptr[0] != '*') {
                    ++ptr;
                }
                ++ptr;

                int i = 0;
                while(ptr[i] != ')') {
                    ++i;
                }
                ptr[i] = '\0';
                if(strcmp(ptr, lookAhead->value) == 0){

                    lookAhead = lookAhead->next;
                    strcat(copy, ")");
                    while(strcmp(lookAhead->value, "{") != 0) {
                        strcat(copy, lookAhead->value);
                        strcat(copy, " ");
                        lookAhead = lookAhead->next;
                    }
                    strcat(copy, ";");
                    strcpy(current->value, copy);
                    break;
                }
                lookAhead = lookAhead->next;
            }
        }
        current = current->next;
    }
}

void printToFile(List *object, char *outFileName) {
    FILE* outfp = fopen(outFileName, "w");
    int tabCount = 0;
    int i, loopFlag = 0;
    while(object->next != NULL) {
        if(strstr(object->value, "void constructor")) {
            fprintf(outfp, "\n");
            tabCount = 0;
        }
        fprintf(outfp, "%s", object->value);
        if((specialPrintChar(object->next->value) == 0) && (specialPrintChar(object->value) == 0)) {
            fprintf(outfp, " ");
        }
        if(strcmp(object->value, "for") == 0) {
            loopFlag = 1;
        }
        if(strstr(object->value, ">") && strstr(object->value, "#")) {
            fprintf(outfp, "\n");
            for(i = 0; i < tabCount; ++i) {
                fprintf(outfp, "\t");
            }
        } else if(strstr(object->value, "\"") && strstr(object->value, "#")) {
            fprintf(outfp, "\n");
            for(i = 0; i < tabCount; ++i) {
                fprintf(outfp, "\t");
            }
        } else if(strcmp(object->value, "{") == 0) {
            ++tabCount;
            loopFlag = 0;
            fprintf(outfp, "\n");
            for(i = 0; i < tabCount; ++i) {
                if(strcmp(object->next->value, "}") == 0) {
                    ++i;
                }
                fprintf(outfp, "\t");
            }
        } else if(strcmp(object->value, "}")  == 0){
            --tabCount;
            if(!(strstr(object->next->value, ";"))) {
                fprintf(outfp, "\n");
                for(i = 0; i < tabCount; ++i) {
                    if(strcmp(object->next->value, "}") == 0) {
                        ++i;
                    }
                    fprintf(outfp, "\t");
                }
            }
        } else if(strstr(object->value, ";")) {
            if(loopFlag == 0) {
                fprintf(outfp, "\n");
                for(i = 0; i < tabCount; ++i) {
                    if(strcmp(object->next->value, "}") == 0) {
                        ++i;
                    }
                    fprintf(outfp, "\t");
                }
            }
        } else if(strstr(object->value, "*/")) {
            fprintf(outfp, "\n");
            for(i = 0; i < tabCount; ++i) {
                if(strcmp(object->next->value, "}") == 0) {
                    ++i;
                }
                fprintf(outfp, "\t");
            }
        }

        List *toFree = object;
        object = object->next;
        free(toFree->value);
        free(toFree);
    }
    fprintf(outfp, "%s", object->value);
    free(object->value);
    free(object);
    fclose(outfp);
}

int specialPrintChar(char *value) {
    if(strstr(value, "(")) {
        return 1;
    } else if(strstr(value, ")")) {
        return 1;
    } else if(strstr(value, ",")) {
        return 1;
    } else if(strstr(value, "+")) {
        return 1;
    } else if(strstr(value, "-")) {
        return 1;
    } else if(strcmp(value, "/") == 0) {
        return 1;
    } else if(strcmp(value, "*") == 0) {
        return 1;
    } else if(strstr(value, "}")) {
        return 1;
    } else if(strstr(value, ";")) {
        return 1;
    }
    return 0;
}

void printObjects(List * object) {
    if(object != NULL) {
        printf("%s\n", object->value);
        printObjects(object->next);
    }
}
