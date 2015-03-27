/* Dai-HW2.c-- Implementation of stdlib.h with execv()
	int system(const char *command)
   AUTHOR:Ruikang Dai
   DATE:Sat Feb 28 16:54:03 EST 2015
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

int system(const char *command);
int makeargv(const char * command, const char *delimiters, char ***argvp);
void freemakeargv(int argn, char **argv);
void errorInfo(int error);

// The pid_t variable to distinguish parent and child
pid_t pID;

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
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	system("deadlock");
	return 0;
}
#endif
// The signal controling variable
struct sigaction act, old_act;
// The Ctrl-c Signal ignore function
void ignoreSignal() {
	// Find current SIGINT handler
	if (sigaction(SIGINT, NULL, &old_act) == -1)
		perror("Failed to get old handler for SIGINT");
	// Set new SIGINT handler to ignore
	act.sa_handler = SIG_IGN;
	if (sigaction(SIGINT, &act, NULL) == -1)
	    	perror("Failed to ignore");
}
// The Ctrl-c Signal restore function
void restoreSignal() {
	if (sigaction(SIGINT, &old_act, NULL) == -1)
	    	perror("Failed to restore");
}
// The function to restore file pointers
void restoreFilePointers() {
	freopen("/dev/tty","a+b",stdout);
	freopen("/dev/tty","a+b",stderr);
	freopen("/dev/tty","a+b",stdin);
}

// This is a function simulate the system function
// Take one string command argument to excute it in shell
// return 0, if success, otherwise return -1
int system(const char *command) {
	int i;
	// The number of argments in the command
	int argn;
	// The vector of argments in the command
	char **argv;
	// The delimiters that seperate the command
	char delim[] = " \t";
	
	if ((argn = makeargv(command, delim, &argv)) <= 0) {
		fprintf(stderr, "Failed to construct an argument array\n");
		return -1;
	} 
	// Ignore interrupt signal
	ignoreSignal();
	// back file Descriptors
	// backupFileDescriptors();
	pID = fork();
	// Child process
	if (pID == 0) {
		// Restore in child process
		restoreSignal();
		restoreFilePointers();
		// Start to excute program
		if (execvp(argv[0],argv) == -1) {
			errorInfo(errno);
			fprintf(stderr, "\nGive up execvp and try execv\n");
			if (execv(argv[0],argv) == -1) {
				errorInfo(errno);
				fprintf(stderr,"\nI cannot handle this\n");
			}
		}
	}
	// Parent process wait child process
	else if (pID > 0) {
		int childpid = wait(NULL);
		if (childpid == -1)
			perror("Failed to wait for child");
	}
	else { 
		fprintf(stderr,"fork() failed!");
		return -1;
	}
	// Restore interrupt signal
	restoreSignal();
	// Free memories in heap
	freemakeargv(argn,argv);
	return 0;
}
// Implementation of freemakeargv varibles in heap
void freemakeargv(int argn, char **argv) {
	if (argv == NULL)
		return;
	free(argv);
}
// The function that separated command to a vector
// const char * command: the input command string
// const char * delimiters: the characters that separates the variables
// char ***argvp: the vector pointer of arguments
// return the number of arguments if success, otherwise return -1
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
		strcpy(command_t, command);
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
// The error information if exec function returns
void errorInfo(int error) {
	switch(error) {
	case E2BIG:
		fprintf(stderr, "Size of new process's argument list and environment list is greater than system-imposed limit of ARG_MAX bytes!");
		break;
	case EACCES:
		fprintf(stderr, "Search permission on directory in path prefix of new process is denied, new process image file execution permission is denied, or new process image file is not a regular file and cannot be executed");
		break;
	case EINVAL:
		fprintf(stderr, "New process image file has appropriate permission and is in a recognizable executable binary format, but system cannot execute files with this format");
		break;
	case ENAMETOOLONG:
		fprintf(stderr, "The length of path or file exceeds PATH_MAX, or a pathname component is longer than NAME_MAX");
		break;
	case ENOENT: 
		fprintf(stderr, "Component of path or file does not name an existing file, or path or file is an empty string");
		break;
	case ENOEXEC:
		fprintf(stderr, "Image file has appropriate access permission but has an unrecognized format (does not apply to execlp or execvp)");
		break;
	case ENOTDIR:
		fprintf(stderr, "A component of the image file path prefix is not a directory");
		break;
	default:
		fprintf(stderr, "I don't know what's wrong.");
		break;
	}
}
