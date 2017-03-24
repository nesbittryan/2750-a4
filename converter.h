#ifndef CONVERTER_H
#define CONVERTER_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

/*
* Linked list which is used for storing each
* part of the cplus file split by special chars
*/
struct LinkedList {
    char *value;
    struct LinkedList *next;
};

/*
* Used to keep track of the first object, to
* avoid losing it
*/
struct HeadOfList {
    struct LinkedList *firstObject;
};

typedef struct LinkedList List;
typedef struct HeadOfList Head;

/*
* IN: Input cplus file name
* This init's the list and fills it with the
* parsed file pieces
* OUT: malloc'd Head pointer
*/
Head *initList(char *inFile);

/*
* IN: Ptr to buffer to be cleared
* This sets every character in the buffer to \0
*/
void clearBuffer(char * buffer);

/*
* IN: Ptr to buffer, character to add
* This adds the character to the end of the buffer
* OUT: New buffer
*/
char *addToBuffer(char * buffer, char ch);

/*
* IN: Ptr to the value of new object
* Creates an linked list object with value passed in
* OUT: Ptr to malloc'd List object
*/
List *createObject(char *value);

/*
* IN: Ptr to list, ptr value to add
* Creates an object using value and then addConstructors
* it to the end of the List pointed at
*/
void addObject(List *object, char *value);

/*
* IN: Ptr to head
* This function begins the convertion to C,
* and calls many other functions
* OUT: Ptr to head
*/
Head *convertToC(Head * head);

/*
* IN: Ptr to structname, Ptr to current object
 This goes through each struct and performs other functions
 * on them
*/
void convertStructs(char * structName, List *object);

/*
* IN: Ptr to str, ptr to structname, Ptr to current object, int flag if structfound
* this creates a function prototype in the ptr format which is passed
* back though the newVal ptr
*/
void funcPrototype(char * newVal, char *structName, List *current, int structFound);

/*
* IN: Ptr to change, Ptr to struct name, Ptr to current object
* This changes the name of to change to make them all unique,
* uses parameters and current struct name
*/
void changeName(char * toChange, char *structName, List *current);

/*
* IN: Ptr to list of functions, Ptr to current
* This extracts the actual function bodies from the structs,
* adding them all head to tail in the func list
* OUT: A list of functions
*/
void removeFuncFromStructs(List * funcList, List *current);

/*
* IN: Ptr to object before current, current, and an int flag
* Depending if flag is 1 or 0, removes current or current->next
*/
void removeFromList(List *beforeCurrent, List *current);

/*
* IN: Ptr to structname, and current object
* Goes through the list creating a constructor, and
* returning it
* OUT: constructor for structname
*/
List *constructorCreator(char *structName, List *current);

/*
* IN: Ptr to current
* Checks if the object needs a constructor, returns 1
* if it does, 0 if it doesnt
* OUT: int flag
*/
int needConstructor(List * current);

/*
* IN: Ptr to current object
* Skips to the next object until it is not still in a comment
* OUT: Ptr to next object
*/
List *skipComment(List *current);

/*
* IN: Ptr to current object
* Skips to the next object until it is not still in a quote
* OUT: Ptr to next object
*/
List *skipQuotes(List * current);

/*
* IN: Ptr to list, Ptr to object to add
* Adds the object to the end of the list
*/
void addToEnd(List *list, List *toAdd);

/*
* IN: Ptr to head
* Goes through the structs and makes sure their is
* no initilizations
*/
void finishStructs(Head * head);

/*
* IN: Ptr to head
* Goes through each function and checks variables if they argSearch
* local, global, or members. If member it adds appropriate parameter
*/
void finishFunc(Head * head);

/*
* IN: Ptr ot current object
* Goes through the file and parses to make a list containing all members
* of that struct, and then adds that to a list of lists
* OUT: List of lists of strings
*/
char ***findMembers(List *current);

/*
* IN: Ptr to object with value "class"
* Changes the word to struct
*/
void classToStruct(List *object);

/*
* IN: Ptr to current object
* Parses through function and returns a list of local variables in it
* OUT: List of strings
*/
char **findLocals(List *current);

/*
* IN: Ptr to current object
* Parses through function and returns a list of paramters in it
* OUT: List of strings
*/
char **findParameters(List *current);

/*
* IN: Local list, members list, param list, current object
* Searches through and if it finds if a variable is local, member or
* parameter, and takes appropriate action
*/
void findReplaceVariables(char ** localList, char ** structMembers, char ** paramList, List *current);

/*
* IN: Ptr ot current
* Adds the constructors into the file
*/
void addConstructors(List *current);

/*
* IN: Ptr to current
* Searches through and creates a list of constructors
* OUT: List of constructors
*/
char ** getConstructors(List *current);

/*
* IN: Ptr to add after, itemname, structname
* Inserts the constructors after (to add After), constructors are made
* using the two char * paramters
*/
void insertConstructors(List *toAddAfter, char *itemName, char *structName);

/*
* IN: Ptr to current
* Searches through and creates a list of functions
* OUT: List of functions
*/
char ** getFuncList(List *current);

/*
* IN: Ptr ot current
* Returns 1 if sequence of finding is '(', ')', '{', 0 if it is not
* that order, or a ';' is present
* OUT: int flag value
*/
int findFlag(List * current);

/*
* IN: Ptr to current
* Searches through and creates a list of struct objects
* OUT: List of struct objects
*/
char **getStructObjList(List * current);

/*
* IN: Ptr to current, funclist, and object list
* Finishes the main by using the func list and object list to
* insert the appropriate names into the right spots. Also adds parameters if it
* needs a struct reference
*/
void finishMain(List *current, char **funcList, char **objList);

/*
* IN: Ptr to string
* Checks value, and if it is a primitive word, returns 1, if
* it is struct, it returns 2, else 0
* OUT: int flag
*/
int isKeyword(char * value);

/*
* IN: Ptr to current, func list
* Looks if a function is called inside a function, and changes the
* the name if found
*/
void funcSearchInFuncs(List *current, char **funcList);

/*
* IN: Ptr to current
* Last function to edit cplus code, this adds the parameters into the
* function ptr prototypes, due to a bug that wouldnt work without them
* OUT:
*/
void finishFuncPtrs(List *current);

/*
* IN: Ptr to current object, output filename
* Opens the output file, and prints out the objects one by one,
* and formats it differently depending on characters in the object
* Also frees the linked list while doing so
*/
void printToFile(List *object, char *outFileName);

/*
* IN: Ptr to string
* Checks if the value contains a special character, and returns 1 if it does,
* else 0
* OUT: int flag
*/
int specialPrintChar(char *value);

/*
* IN: Ptr to current
* Prints out the object one by one
* Used for testing mostly, not actually used in finished product
*/
void printObjects(List *object);

#endif
