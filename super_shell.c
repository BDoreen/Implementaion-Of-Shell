#include "header.h"

int main()
{
	char shCmd[256] = { '\0' }, ** parsedCmd = NULL;
	int numArgs = 0, isSpecial = 0, isLock = 0;
	Node* listLockedCmds = NULL;

	while (1)
	{
		isSpecial = 0;
		numArgs = 0;

		printf("\nSuperShell>");

		fgets(shCmd, 256, stdin);
		numArgs = cntWords(shCmd);
		if (numArgs > 3)
		{
			printf("Only 3 arguments are allowed.\n");
			continue;
		}

		/*separate each word in the command to different cell in the array*/
		parsedCmd = parseCmd(shCmd, numArgs, &listLockedCmds);
		isLock = checkLock(parsedCmd[0], &listLockedCmds);//check if command is locked
		if (isLock == 1)
			printf("The command %s is now locked!\n", parsedCmd[0]);
		else
		{
			isSpecial = callSpecialCmd(parsedCmd, numArgs, &listLockedCmds);

			if (isSpecial == 0)//if command isn't a special command, execute it from PATH
				callExecvp(parsedCmd, numArgs, &listLockedCmds);
		}
		parsedCmd = freeParsed(parsedCmd, numArgs);//free memory
	}
	return 0;
}
