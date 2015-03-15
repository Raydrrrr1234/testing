/* Dai-HW2.c-- Implementation of stdlib.h
	int system(const char *command)
   AUTHOR:Ruikang Dai
   DATE:Sat Feb 28 16:54:03 EST 2015
*/
#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int system(const char *command);
int makeargv(const char * command, const char *delimiters, char ***argvp);
void freemakeargv(int argn, char **argv);

#ifdef DEBUG_makeargv
int main (int argc, char *argv) {
	char delim[] = " \t";
	int i;
	char **myargv;
	int numtokens;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s string\n", argv[0]);
		return 1;
	}
	if ((numtokens = makeargv(argv[1], delim, &myargv)) == -1) {
		fprintf(stderr, "Failed to construct an argument array for %s\n", argv[1]);
		return 1;
	}
	printf("The argument array contains:\n");
	for (i = 0; i < numtokens; i++) 
		printf("%d:%s\n", i, myargv[i]);
	freemakeargv(numtokens, myargv);
	return 0;
}
#endif
int system(const char *command) {
	int i;
	// The number of argments in the command
	int argn;
	// The vector of argments in the command
	char **argv;
	// The delimiters that seperate the command
	char delim[] = " \t";
 
	if((argn = makeargv(command, delim, &argv)) == NULL) {
		fprintf(stderr, "Failed to construct an argument array\n");
	} 
	
	execvp(program_name,argv);
	// Free memories in heap
	freemakeargv(argn,argv);
}
void freemakeargv(int argn, char **argv) {
	if (argv == NULL)
		return;
	if (*argv != NULL)
		while(argn != 0)
			free(argv[--argn]);
	free(argv);
}
int makeargv(const char * command, const char *delimiters, char ***argvp) {
	int argn = 0;
	int error;
	int numtokens;
	// strtok_r thread safety argument
	char *lasts;
	// Input error checking
	if ((command == NULL) || (delimiters == NULL) || (argvp == NULL)) {
		errno = EINVAL;
		return -1;
	}
	// Copy the command string
	if((char *temp = (char *)malloc(sizeof(char)*strlen(command))) == NULL) {
		printf("Not enough memory!!(system function)");
		return -1;
	}
	// Free the temp clauses seperate tokens
	{
		char *temp_for_delete = temp;
		// count the number of tokens in command
		strcpy(temp, command);
		if(strtok_r(temp, delimiters,&lasts) != NULL) {
			while(strtok_r(NULL, delimiters,&lasts) != NULL) {
				argn++;
			}
		}
		// Create argument array for ptrs to the tokens	
		if((char **argv = (char*)malloc(sizeof(char*)*argn+1)) == NULL) {
			error = errno;
			free(temp_for_delete);
			errno = error;
			printf("Not enough memory!!(system function)");
			return -1;
		}
		if(numtokens == 0)
			free(temp_for_delete);
		else {
			// Insert pointers to tokens into the argument array 
			strcpy(temp, command);
			char *program_name = strtok_r(temp, delimiters,&lasts);
			for(int i = 0; i < argn; i++) {
				char *arg = strtok_r(NULL, delimiters,&lasts);
				if((argv[i] = (char*)malloc(sizeof(char)*strlen(arg)+1)) == NULL) {
					printf("Not enough memory!!(system function)");
					return -1;
				}
				strcpy(argv[i],arg);
			}
			// put in a finall NULL pointer
			argv[argn] = NULL;
			free(temp_for_delete);
		}
	}
	*argvp = argv;
	return argn;
}
