/* systemFunc.c--
   AUTHOR:Ruikang Dai
   DATE:Sat Mar 14 11:42:39 EDT 2015
*/
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_CANON
#define max_canon 8192
#endif

int runproc(char *cmd);
void showhistory(FILE *f);

int main(int argn, char **argv) {
	char cmd[MAX_CANON];
	int history = 1;
	
	if (argn == 1)
		history = 0;
	else if ((argn > 2) || strcmp(argv[1], "history")) {
		fprintf(stderr, "Usage: %s [history]\n", argv[0]);
		return 1;
	}
	while (fgets(cmd, MAX_CANON, stdin) != NULL) {
		if (*(cmd + strlen(cmd) - 1) == '\n')
			*(cmd + strlen(cmd) - 1) = 0;
		if (history && !strcmp(cmd, "history"))
			showhistory(stdout);
		else if (runproc(cmd)) {
			perror("Failed to execute command");
			break;
		}
	}
	printf("\n\n>>>>>The list of commands executed is: \n");
	showhistory(stdout);
	return 0;
}

