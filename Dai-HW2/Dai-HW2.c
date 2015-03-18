/* Dai-HW2.c-- Implementation of stdlib.h
	int system(const char *command)
   AUTHOR:Ruikang Dai
   DATE:Sat Feb 28 16:54:03 EST 2015
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
int system(const char *command);
int makeargv(const char * command, const char *delimiters, char ***argvp);
void freemakeargv(int argn, char **argv);
void errorInfo(int error);

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
#ifdef DEBUG
int main(int argn, char **argv) {
	system(argv[1]);
	system("outputIDs");
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
#ifdef DEBUG
	fprintf(stderr, "%s\n", command);
#endif 
	if ((argn = makeargv(command, delim, &argv)) <= 0) {
		fprintf(stderr, "Failed to construct an argument array\n");
		return -1;
	} 
#ifdef DEBUG
	for (i = 0; i < argn; i++) {
		fprintf(stderr, "%s\n", argv[i]);
	}
#endif	
	if (execvp(argv[0],argv+1) == -1) 
		errorInfo(errno);
	// Free memories in heap
	freemakeargv(argn,argv);
	return 1;
}
void freemakeargv(int argn, char **argv) {
	if (argv == NULL)
		return;
	free(argv);
}
int makeargv(const char * command, const char *delimiters, char ***argvp) {
	// strtok_r thread safety argument
	char *lasts;
	// Temporary variables
	int argn = 0;
	int error;
	char *command_t;
	char **argv;
	int i;
	// Input error checking
	if ((command == NULL) || (delimiters == NULL) || (argvp == NULL)) {
		errno = EINVAL;
		return -1;
	}
	// copy the command to command_t
	if ((command_t = (char*)malloc(strlen(command)*sizeof(char))) == NULL) {
		perror("Not enough memory!!(system function)");
		return -1;
	}
	strcpy(command_t, command);
	// count the number of tokens in command
	if (strtok_r(command_t, delimiters,&lasts) != NULL) {
		argn++;
		while(strtok_r(NULL, delimiters,&lasts) != NULL) {
			argn++;
		}
	}
	if (argn == 0) {
		perror("No command!");
		return -1;
	}
	// Create argument array for ptrs to the tokens	
	if ((argv = (char**)malloc(sizeof(char*)*argn+1)) == NULL) {
		perror("Not enough memory!!(system function)");
		return -1;
	}
	else {
	// Insert pointers to tokens into the argument array 
		argv[0] = strtok_r(command_t, delimiters,&lasts);
		for (i = 1; i < argn; i++) {
			argv[i] = strtok_r(NULL, delimiters,&lasts);
		}
		// put in a finall NULL pointer
		argv[argn] = NULL;
	}
	free(command_t);
	*argvp = argv;
	return argn;
}
void errorInfo(int error) {
	switch(error) {
	case E2BIG:
		fprintf(stderr, "size of new process's argument list and environment list is greater than system-imposed limit of ARG_MAX bytes!");
		break;
	case EACCES:
		fprintf(stderr, "search permission on directory in path prefix of new process is denied, new process image file execution permission is denied, or new process image file is not a regular file and cannot be executed");
		break;
	case EINVAL:
		fprintf(stderr, "new process image file has appropriate permission and is in a recognizable executable binary format, but system cannot execute files with this format");
		break;
	case ENAMETOOLONG:
		fprintf(stderr, "the length of path or file exceeds PATH_MAX, or a pathname component is longer than NAME_MAX");
		break;
	case ENOENT: 
		fprintf(stderr, "component of path or file does not name an existing file, or path or file is an empty string");
		break;
	case ENOEXEC:
		fprintf(stderr, "image file has appropriate access permission but has an unrecognized format (does not apply to execlp or execvp)");
		break;
	case ENOTDIR:
		fprintf(stderr, "a component of the image file path prefix is not a directory");
		break;
	}
}
