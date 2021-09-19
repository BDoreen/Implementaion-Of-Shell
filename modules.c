#include "header.h"

/*input:	shCmd - shell command which contains parameters
  goal:		separate the parameters
  output:	char** - each cell contains one parameter
  alg:		In order to know how many characters to copy into cmdByWords[i],
			function counts lenOfWord by identifying white-spaces.
			Also, in order to know from where to copy the next parameter into cmdByWords[i+1],
			function counts skipLetters. */
char** parseCmd(char* shCmd, int numArgs, Node** listLockedCmds)
{
	int lenOfWord = 0, skipLetters = 0, nwords = 0, i = 0, lenCmd = 0;
	char** parsedCmd = NULL;

	lenCmd = (strlen(shCmd) + 1);

	parsedCmd = (char**)calloc((numArgs + 2), sizeof(char*));
	if (parsedCmd == NULL)
	{
		printf("malloc failed.");
		delList(listLockedCmds);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < numArgs + 1; i++)
	{
		parsedCmd[i] = (char*)calloc(11, sizeof(char));
		if (parsedCmd[i] == NULL)
		{
			printf("malloc failed.");
			i--;
			for (; i >= 0; i--)
				free(parsedCmd[i]);
			delList(listLockedCmds);
			exit(1);
		}
	}

	for (i = 0; i < lenCmd; i++)
	{
		if (shCmd[i] == ' ' || shCmd[i] == '\0')
		{
			/*strncpy copies up to n characters (n characters are one word)*/
			strncpy(parsedCmd[nwords], shCmd + skipLetters, lenOfWord);
			lenOfWord = -1; //nullify writeLetters
			nwords++; //num of words that been read
			skipLetters = i + 1;
		}
		lenOfWord++; //one char been read.
	}
	return parsedCmd;
}

/*Returns number of words in a sentence*/
int cntWords(char* shCmd)
{
	int len = 0, num = 0, i = 0;

	len = strlen(shCmd);
	shCmd[len - 1] = '\0';

	for (i = 0; i < len - 1; i++)
		if (shCmd[i] == ' ')
			num++;

	return num;
}

/*function executes a command that accommodates PATH, by using fork.
  If command is not in PATH, "Not supported" is printed.*/
void callExecvp(char** parsedCmd, int numArgs, Node** listLockedCmds)
{
	int pid = 0;
	pid = fork();

	switch (pid)
	{
	case -1:
		perror("fork");
		freeParsed(parsedCmd, numArgs);
		delList(listLockedCmds);
		exit(1);
	case 0:
		execvp(parsedCmd[0], parsedCmd);
		printf("Not supported\n");
		freeParsed(parsedCmd, numArgs);
		delList(listLockedCmds);
		exit(1);
	default:
		break;
	}
	wait(NULL);
}

/*Function mallocs memory for node and initials it.*/
Node* createNode(int len)
{
	Node* pnode = NULL;

	pnode = (Node*)malloc(sizeof(Node));

	if (!pnode)
	{
		perror("Create node");
		return NULL;
	}

	pnode->name = (char*)calloc(len, sizeof(char));
	if (pnode->name == NULL)
	{
		perror("Create node");
		return NULL;
	}
	pnode->time = 0;
	pnode->next = NULL;

	return pnode;
}

/*lock command 'cmdToLock' of use*/
int lockCmdForTime(char* cmdToLock, int lockDuration, Node** listLockedCmds)
{
	Node* cur = NULL, * prev = NULL;
	int len = 0, inList = 0;

	len = strlen(cmdToLock);

	if ((*listLockedCmds) == NULL)
	{
		(*listLockedCmds) = createNode(len);
		if (*listLockedCmds == NULL)//memory allocation failed
			return -1;
		insertLockingVals(*listLockedCmds, cmdToLock, lockDuration);
	}
	else
	{
		cur = *listLockedCmds;
		while (cur != NULL)
		{
			if (strcmp(cmdToLock, cur->name) == 0)
			{
				cur->time = cur->time + lockDuration;//increase time of locking
				inList = 1;
				return 1;
			}
			prev = cur;
			cur = cur->next;
		}

		if (inList == 0)
		{
			prev->next = createNode(len);
			if (prev->next == NULL)//memory allocation failed
				return -1;
			prev = prev->next;
			insertLockingVals(prev, cmdToLock, lockDuration);
		}
	}
}

void insertLockingVals(Node* toLock, char* nameCmd, int lockDuration)
{
	strcpy(toLock->name, nameCmd);
	gettimeofday(&current_time, NULL);
	toLock->time = lockDuration + current_time.tv_sec;
	toLock->next = NULL;
}

/*Function encrypts the file "from" by adding "code" to the ascii value of the characters in the file.*/
int encryptFile(char* from, int code, char* to)
{
	int fdFrom = 0, fdTo = 0, rbytes = 0, wbytes = 0;
	char ch = '\0';

	if ((fdFrom = open(from, O_RDONLY)) == -1)
	{
		perror("open\n");
		return -1;
	}

	if ((fdTo = open(to, O_WRONLY | O_CREAT, 0664)) == -1)
	{
		perror("out\n");
		close(fdFrom);
		return -1;
	}

	if (((rbytes = read(fdFrom, &ch, 1))) == -1)
	{
		perror("read\n");
		close(fdFrom);
		close(fdTo);
		return -1;
	}


	while (rbytes != 0)
	{
		/*Encrypt only if a letter or digit*/
		if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
		{
			ch += code;
			ch %= 256;
		}


		if ((wbytes = write(fdTo, &ch, 1)) == -1)
		{
			perror("write");
			close(fdFrom);
			close(fdTo);
			return -1;
		}

		if (wbytes != 1)
		{
			printf("Error write");
			close(fdFrom);
			close(fdTo);
			return -1;
		}

		if (((rbytes = read(fdFrom, &ch, 1))) == -1)
		{
			perror("read\n");
			close(fdFrom);
			close(fdTo);
			return -1;
		}
	}

	close(fdFrom);
	close(fdTo);
	return 1;
}

/*Function decrypts the file "from" by decreasing "code" from the ascii value of the characters in the file.*/
int decryptFile(char* from, int code, char* to)
{
	int fdFrom = 0, fdTo = 0, rbytes = 0, wbytes = 0;
	char ch = '\0';

	if ((fdFrom = open(from, O_RDWR)) == -1)
	{
		perror("open");
		return -1;
	}
	if ((fdTo = open(to, O_WRONLY | O_CREAT, 0664)) == -1)
	{
		perror("open");
		close(fdFrom);
		return -1;
	}

	if (((rbytes = read(fdFrom, &ch, 1))) == -1)
	{
		perror("read");
		close(fdFrom);
		close(fdTo);
		return -1;
	}

	while (rbytes != 0)
	{

		/*Decrypt only if a letter or digit*/
		if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
		{
			ch -= code;
			ch %= 256;
		}

		if ((wbytes = write(fdTo, &ch, sizeof(ch))) == -1)
		{
			perror("write");
			close(fdFrom);
			close(fdTo);
			return -1;
		}
		if (wbytes != 1) {
			printf("ERROR: write");
			close(fdFrom);
			close(fdTo);
			return -1;
		}
		if (((rbytes = read(fdFrom, &ch, 1))) == -1)
		{
			perror("read");
			close(fdFrom);
			close(fdTo);
			return -1;
		}
	}

	close(fdFrom);
	close(fdTo);
	return 1;
}

/*Function scans the file 'from' and finds three ABC letters that occur most of the time.
  Function prints their frequency, and if the order of those three letters is a-e-o
  than it prints "Good Letter Frequency*/
int letterFreq(char* from)
{
	int fdFrom = 0, rbytes = 0, iMax = 0, i = 0, j = 0, cntLetters = 0;
	int cntArr[26] = { 0 }, iArr[3] = { 0 };
	double letterPercent = 0;
	char ch = '\0';

	if ((fdFrom = open(from, O_RDONLY)) == -1)
	{
		perror("letterFreq open");
		return -1;
	}

	if ((rbytes = read(fdFrom, &ch, 1)) == -1)
	{
		perror("letterFreq read");
		close(fdFrom);
		return -1;
	}

	/*Nullify cnt_arr to - 1 for better counting.*/
	for (i = 0; i < 26; i++)
		cntArr[i] = 0;

	/*count number of occurrences of each letter in the ABC*/
	while (rbytes > 0)
	{
		if (ch <= 'z' && ch >= 'a') //if lower case
		{
			ch %= 'a';
			cntArr[(int)ch]++;
		}
		else if (ch <= 'Z' && ch >= 'A') //if upper case
		{
			ch %= 'A';
			cntArr[(int)ch]++;
		}
		if ((rbytes = read(fdFrom, &ch, 1)) == -1)
		{
			perror("letterFreq read");
			close(fdFrom);
			return -1;
		}
	}

	for (i = 0; i < 26; i++) //counts number of different letters in file
		cntLetters += cntArr[i];

	letterPercent = (100.0 / cntLetters);

	/*Find maximum three times, and print the letter and it's distribution */
	for (j = 0; j < 3; j++)
	{
		/*Find maximum*/
		for (i = 1; i < 26; i++)
		{
			if (cntArr[iMax] < cntArr[i])
				iMax = i;
		}
		/*Print the letter and it's distribution*/
		if (cntArr[iMax] > 0)
		{
			//save the letter that was found to know if Good Frequency is on the paper
			iArr[j] = (iMax + 97);
			//print the letter and it's distribution
			printf("%c - %.1f%\n", iArr[j], ((cntArr[iMax]) * letterPercent));
			//don't count that max again
			cntArr[iMax] = 0;
			iMax = 0;
		}
		/*if not greater than 0, than there are no more letters so end function.*/
		else
			return 1;
		iMax = 0;
	}

	if (iArr[0] == 'e' && iArr[1] == 'a' && iArr[2] == 'o')
		printf("Good Letter Frequency\n");

	close(fdFrom);
	return 1;
}

/*Function scans each word in file from and transforms readWord[index] to uppercase.*/
int uppercaseByIndex(char* from, char* to, int index)
{
	int fdFrom = 0, fdTo = 0, rbytes = 1, wbytes = 0, j = 0, k = 0;
	char readWord[100] = { '\0' }, ch = '\0';

	/*check that index is acceptable*/
	if (index > 20)
		return errMsg("index must be less than 20", fdFrom, fdTo);

	/*open files*/
	if ((fdFrom = open(from, O_RDONLY)) == -1)
		return errMsg("open from", fdFrom, fdTo);
	if ((fdTo = open(to, O_RDWR | O_CREAT, 0664)) == -1)
		return	errMsg("uppercase open to", fdFrom, fdTo);

	/*read one letter*/
	if ((rbytes = read(fdFrom, &ch, 1)) == -1)
		return	errMsg("uppercase read", fdFrom, fdTo);

	while (rbytes > 0)
	{
		/*nullify array each iteration*/
		for (k = 0; k < 20; k++)
			readWord[k] = '\0';

		/*read a word*/
		for (j = 0; (rbytes > 0); j++)
		{
			readWord[j] = ch;

			/*if current letter is whitespace, than paste it to array and finish reading a word*/
			if ((ch == ' ') || (ch == '\n'))
				break;

			/*else, continue reading the word*/
			if ((rbytes = read(fdFrom, &ch, 1)) == -1)
				return	errMsg("uppercase read", fdFrom, fdTo);
		}

		/*if requested index is a lowercase, than transfer to uppercase and write to file*/
		if (readWord[index] >= 'a' && readWord[index] <= 'z')
			readWord[index] -= 32;
		wbytes = write(fdTo, readWord, strlen(readWord));

		if (wbytes == -1 || wbytes != strlen(readWord))
			return	errMsg("uppercase write", fdFrom, fdTo);

		/*read another single letter*/
		if ((rbytes = read(fdFrom, &ch, 1)) == -1)
			return	errMsg("uppercase read", fdFrom, fdTo);
	}

	close(fdFrom);
	close(fdTo);
	return 1;
}

/*Function scans each word in file from and transforms readWord[index] to lowercase.*/
int lowercaseByIndex(char* rFile, char* wFile, int index)
{
	int fdFrom = 0, fdTo = 0, rbytes = 1, wbytes = 0, j = 0, k = 0;
	char readWord[100] = { '\0' }, ch = '\0';

	if ((fdFrom = open(rFile, O_RDONLY)) == -1)
		return errMsg("open", fdFrom, fdTo);

	if ((fdTo = open(wFile, O_RDWR | O_CREAT, 0664)) == -1)
		return errMsg("lowercase open_to", fdFrom, fdTo);

	if ((rbytes = read(fdFrom, &ch, 1)) == -1)
		return 	errMsg("lowercase read", fdFrom, fdTo);

	while (rbytes > 0)
	{
		/*nullify array each iteration*/
		for (k = 0; readWord[k] != '\0'; k++)
			readWord[k] = '\0';

		/*Read a single word*/
		j = 0;
		for (j = 0; rbytes > 0; j++)
		{
			readWord[j] = ch;

			/*if ch is whitespace than stop reading (end of word)*/
			if ((ch == ' ') || (ch == '\n'))
				break;

			/*read single letter*/
			if ((rbytes = read(fdFrom, &ch, 1)) == -1)
				return 	errMsg("lowercase read", fdFrom, fdTo);
		}


		if (readWord[index] <= 'Z' && readWord[index] >= 'A')
			readWord[index] += 32;
		wbytes = write(fdTo, readWord, strlen(readWord));
		if (wbytes == -1)
			return 	errMsg("lowercase write", fdFrom, fdTo);

		if (wbytes < strlen(readWord))
			return 	errMsg("Wrong number of letters was written", fdFrom, fdTo);

		if ((rbytes = read(fdFrom, &ch, 1)) == -1)
			return 	errMsg("lowercase read", fdFrom, fdTo);
	}

	close(fdFrom);
	close(fdTo);
	return 1;
}

/*Function writes "num" random ABC letters to 'w_file'*/
int randomFile(int num, char* wFile)
{
	int fdTo = 0, wbytes = 0;
	char randomletter = '\0';

	if ((fdTo = open(wFile, O_RDWR | O_CREAT, 0664)) == -1)
		return 	errMsg("uppercase open", 0, 0);

	while (num > 0)
	{
		randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"[rand() % 52];
		if ((wbytes = write(fdTo, &randomletter, 1)) == -1)
			return 	errMsg("randomFile write", fdTo, 0);

		if (wbytes != 1)
			return 	errMsg("wrong number of letters was written", fdTo, 0);

		num--;
	}

	close(fdTo);
	return 1;
}

/*Function receives two paths, it scans the file in the reading path,
and compresses the words which occur more than 4 times in a row */
int compressFile(char* pathOpen, char* pathSave)
{
	int fdFrom = 0, fdTo = 0, rbytes = 0, wbytes = 0, dupInt = 1, i = 0;
	char ch1 = '\0', ch2 = '\0', dupChar[4] = { '\0' };
	char tPath[256] = { '\0' }, * rPath = NULL, * wPath = NULL;

	/*Get current working directory*/
	getcwd(tPath, sizeof(tPath));
	if (tPath == NULL)
		return freeCompFunc("getenv", rPath, wPath, fdFrom, fdTo);

	//START create path_read
	rPath = (char*)malloc((sizeof(tPath)) + (sizeof(pathOpen)) + 1);
	if (rPath == NULL)
		return freeCompFunc("malloc", rPath, wPath, fdFrom, fdTo);
	strcpy(rPath, tPath);
	strcat(rPath, "/");
	strcat(rPath, pathOpen);
	//END

	//START create path_write
	wPath = (char*)malloc((sizeof(tPath)) + (sizeof(pathSave)) + 1);
	if (!wPath)
		return freeCompFunc("malloc", rPath, wPath, fdFrom, fdTo);
	strcpy(wPath, tPath);
	strcat(wPath, "/");
	strcat(wPath, pathSave);
	//END

	//open files
	if ((fdFrom = open(rPath, O_RDONLY)) == -1)
		return freeCompFunc("path_read", rPath, wPath, fdFrom, fdTo);
	if ((fdTo = open(wPath, O_WRONLY | O_CREAT, 0664)) == -1)
		return freeCompFunc("path_write", rPath, wPath, fdFrom, fdTo);

	if ((rbytes = read(fdFrom, &ch1, 1)) == -1)
		return freeCompFunc("read", rPath, wPath, fdFrom, fdTo);


	while (rbytes > 0)
	{
		wbytes = write(fdTo, &ch1, 1);
		if (wbytes == -1 || wbytes != 1)
			return 	freeCompFunc("write", rPath, wPath, fdFrom, fdTo);

		if ((rbytes = read(fdFrom, &ch2, 1)) == -1)
			return 	freeCompFunc("read", rPath, wPath, fdFrom, fdTo);

		if (rbytes == 0) //EOF
			return 1;

		dupInt = 1;
		for (i = 0; i < (sizeof(int) + 1); i++)
			dupChar[i - 1] = '\0';

		//count duplication
		while (ch1 == ch2)
		{
			dupInt++;

			if ((rbytes = read(fdFrom, &ch2, 1)) == -1)
				return freeCompFunc("read", rPath, wPath, fdFrom, fdTo);

			if (rbytes == 0)
				break;
		}

		if (dupInt > 4)
		{
			sprintf(dupChar, "%d", dupInt);
			if ((wbytes = write(fdTo, dupChar, strlen(dupChar))) == -1)
				return 	freeCompFunc("write", rPath, wPath, fdFrom, fdTo);
		}
		else
		{//write duplications
			if (dupInt != 1)
			{
				for (i = 0; i < dupInt - 1; i++)
				{
					wbytes = write(fdTo, &ch1, 1);
					if (wbytes == -1 || wbytes != 1)
						return 	freeCompFunc("write", rPath, wPath, fdFrom, fdTo);
				}
			}
		}
		ch1 = ch2;
	}
	return 1;
}

/*Function calls special commands. handles all malloc by res.*/
int callSpecialCmd(char** parsedCmd, int numArgs, Node** listLockedCmds)
{
	int i = 0, res = 0;
	const char* const specialCmd[9] = { "encryptFile","decryptFile", "lockCmdForTime",
										"letterFreq", "uppercaseByIndex", "lowercaseByIndex",
										"randomFile", "compressFile", "byebye" };

	//find index of command
	for (i = 0; i < 9; i++) {
		if (strcmp(parsedCmd[0], specialCmd[i]) == 0)
			break; //exit loop
	}

	switch (i)
	{
	case 0:
		if (numArgs != 3) { printf("Not enough arguments.\n"); break; }
		res = encryptFile(parsedCmd[1], atoi(parsedCmd[2]), parsedCmd[3]);
		break;
	case 1:
		if (numArgs != 3) { printf("Not enough arguments.\n"); break; }
		res = decryptFile(parsedCmd[1], atoi(parsedCmd[2]), parsedCmd[3]);
		break;
	case 2:
		if (numArgs != 2) { printf("Not enough arguments.\n"); break; }
		res = lockCmdForTime(parsedCmd[1], atoi(parsedCmd[2]), listLockedCmds);
		break;
	case 3:
		if (numArgs != 1) { printf("Not enough arguments.\n"); break; }
		res = letterFreq(parsedCmd[1]);
		break;
	case 4:
		if (numArgs != 3) { printf("Not enough arguments.\n"); break; }
		res = uppercaseByIndex(parsedCmd[1], parsedCmd[2], atoi(parsedCmd[3]));
		break;
	case 5:
		if (numArgs != 3) { printf("Not enough arguments.\n"); break; }
		res = lowercaseByIndex(parsedCmd[1], parsedCmd[2], atoi(parsedCmd[3]));
		break;
	case 6:
		if (numArgs != 2) { printf("Not enough arguments.\n"); break; }
		res = randomFile(atoi(parsedCmd[1]), parsedCmd[2]);
		break;
	case 7:
		if (numArgs != 2) { printf("Not enough arguments.\n"); break; }
		res = compressFile(parsedCmd[1], parsedCmd[2]);
		break;
	case 8:
		if (numArgs != 0) { printf("Not enough arguments.\n"); break; }
		byeBye(listLockedCmds, parsedCmd, numArgs);
		break;
	default:
		return 0;
	}
	if (res == -1)
	{
		freeParsed(parsedCmd, numArgs);
		delList(listLockedCmds);
		exit(1);
	}
}

/*Function receives name of command and checks if there is a relevant lock on it.*/
int checkLock(char* parsedCmd, Node** listLockedCmds)
{
	Node* temp = NULL;

	temp = *listLockedCmds;
	gettimeofday(&current_time, NULL);

	while (temp != NULL)
	{
		if (strcmp(temp->name, parsedCmd) == 0)
		{
			//check if locking time has passed.
			if (current_time.tv_sec > temp->time)
				return 0;
			else
				return 1;
		}
		temp = temp->next;
	}
	return 0;
}

/*function receives char** parsedCMD and frees memory according to argNum */
char** freeParsed(char** parsedCmd, int argNum)
{
	int i = 0;

	for (i = 0; i < argNum + 1; i++)
		free(parsedCmd[i]);
	free(parsedCmd);
}

/*prints error msg, closes fds and returns -1 for specialCmd func to handle.*/
int errMsg(char* msg, int fdIn, int fdOut)
{
	perror(msg);
	close(fdIn);
	close(fdOut);
	return -1;
}


/*function frees memory of rPath and wPath.
  Also it returns -1 for specialCmd func to handle.*/
int freeCompFunc(char* msg, char* rPath, char* wPath, int fdIn, int fdOut)
{
	perror(msg);
	free(rPath);
	free(wPath);
	close(fdIn);
	close(fdOut);
	return -1;
}

/*function frees memory and exits.*/
void byeBye(Node** list, char** parsedCmd, int argNum)
{
	delList(list);
	freeParsed(parsedCmd, argNum);
	exit(0);
}

/*Function frees memory of one diretion list.
  AlG: from head tp tail.*/
void delList(Node** listLockedCmd)
{
	Node* temp = NULL, * prev = NULL;

	temp = *listLockedCmd;
	while (temp != NULL)
	{
		prev = temp;
		temp = temp->next;
		free(prev->name);
		free(prev);
	}
	*listLockedCmd = NULL;
}

