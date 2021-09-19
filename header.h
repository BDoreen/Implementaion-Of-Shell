#ifndef header
#define header

#define _CRT_SECURE_NO_WARNINGS
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>

typedef struct Node
{
	char* name;
	int time;
	struct Node* next;
}Node;

struct timeval current_time;

/*out of PATH functions*/

int encryptFile(char* from, int code, char* to);
int decryptFile(char* from, int code, char* to);
int lockCmdForTime(char* cmdToLock, int lockDuration, Node** listLockedCmd);
int uppercaseByIndex(char* from, char* to, int index);
int lowercaseByIndex(char* rFile, char* wFile, int index);
int letterFreq(char* from);
int randomFile(int num, char* wFile);
int compressFile(char* pathOpen, char* pathSave);
void byeBye(Node** listLockCmd, char** listparsedCmd, int argNum);
void insertLockingVals(Node* toLock, char* nameCmd, int lockDuration);

int cntWords(char* shCmd);
char** parseCmd(char* shCmd, int argNum, Node** listLockedCmd);
Node* createNode(int len);
int callSpecialCmd(char** parsedCmd, int argNum, Node** listLockedCmd);
void callExecvp(char** parsedCmd, int argNum, Node** listLockedCmd);
int checkLock(char* cmd, Node** listLockedCmd);
int freeCompFunc(char* msg, char* rPath, char* wPath, int fdIn, int fdOut);
void delList(Node** listLockedCmd);
char** freeParsed(char** parsedCmd, int argNum);
int errMsg(char* msg, int fdIn, int fdOut);

#endif // !header

