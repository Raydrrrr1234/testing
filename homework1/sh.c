/* sh.c-- CS 571 Homework 1
   AUTHOR:Ruikang Dai
   DATE:Sun Sep 13 22:44:27 EDT 2015
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <memory.h>

//Marcos:
#define IDENT_COUNT 2
#define MAX_COMMAND_COUNT 512
#define STDIN 0
#define STDOUT 1
#define STDERR 2
//Functions:
void welcomeWords();
int runCommand();
int executeCommand(char *command);
int makeargv(char * command, char ***argvp);
void freemakeargv(int argn, char ** argv);
#ifndef DEBUG_makeargv
int main(int argn, char **argv) {

	// Check number of command-line arguments
//	if (argn != 2) {
//		return -1;
//	}

	while (1) {
		runCommand();
	}

	return 0;
}
int runCommand() {
	char command[MAX_COMMAND_COUNT+1];
	welcomeWords();
	read(STDIN,command,MAX_COMMAND_COUNT); 
	executiveCommand(command);
	return 0;
}
const char * ident = "$ ";
void welcomeWords() {
//	char *pwd = get_current_dir_name();
//	char *ident = (char*)malloc(strlen(pwd)+IDENT_COUNT+1);
//	char *ident = (char*)malloc(IDENT_COUNT+1);
//	ident[0] = '$';
//	ident[1] = ' ';
//	ident[2] = 0;
//	strcat(ident,pwd);
	write(STDOUT, ident,strlen(ident));
}
// pid to identify the parent process and child process
pid_t pid;
const char errorMsg[] = "Failed to execute the command\n";
int executiveCommand(char *command) {
	int i;
	// The number of arguments in the command
	int argn;
	// The vector of arguments in the command
	char **argv;
	// The delimiters that seperate the command
	
	if ((argn = makeargv(command, &argv)) <= 0) {
		write(STDERR, errorMsg, strlen(errorMsg));
		return -1;
	}
	
	pid = fork();
	// Child process
	if (pid == 0) {
		// Start to execute program
		if (execvp(argv[0],argv) == -1) {
			//Try another exec
			if (execv(argv[0], argv) == -1) {
				write(STDERR, errorMsg, strlen(errorMsg));
			}
		}
	}
	// Parent process wait child process
	else if (pid > 0) {
		pid_t childpid = wait(NULL);
		if (childpid == -1)
			write(STDERR, errorMsg, strlen(errorMsg));
	}
	else {
		return -1;
	}
	// Free memories in heap
	freemakeargv(argn, argv);
	return 0;
}
#endif

const char initial_error[] = "Initial error!!";
const char no_command[] = "No command";
const char no_enough_memory[] = "No enough memory!!";
int makeargv(char * command, char ***argvp) {
	char *nextStart = command;
	char **argv;
	int argn = 0;
	int len = strlen(command);
	size_t offset;
	int i;
	// Input error checking
	if ((command == NULL) || (argvp == NULL)) {
		write(STDERR, initial_error, strlen(initial_error));
		return -1;
	}
	// Empty the starting blank space
	while (*nextStart == ' ' || *nextStart == '\t') nextStart++;
	// Count the number of argments in command
	offset = strcspn(command, " \t");
	while (*nextStart != 0) {
		// Found command
		if (offset >= 1) {
			argn++;
		}
		// Go ahead
		nextStart += (offset+1);
		offset = strcspn(nextStart, " \t");
	}
	// Error check
	if (argn == 0) {
		write(STDERR, no_command, strlen(no_command));
		return -1;
	}
	// Create argument array for argv
	if (((argv = (char**)malloc(sizeof(char*)*(argn+1)))) == NULL) {
		write(STDERR, no_enough_memory, strlen(no_enough_memory));
		return -1;
	}
	else {
		// Make an argv
		i = 0;
		nextStart = command;
		// Empty the starting blank space
		while (*nextStart == ' ' || *nextStart == '\t') nextStart++;
		offset = strcspn(command, " \t");
		while (*nextStart != 0) {
			if (offset >= 1) {
				// Copy the string
				if ((argv[i] = (char*)malloc(sizeof(char)*(offset+1))) == NULL) {
					write(STDERR, no_enough_memory, strlen(no_enough_memory));
					return -1;
				}
				strncpy(argv[i], nextStart, offset);
				argv[i][offset] = 0;
				i++;
			}
			// Go ahead
			nextStart += (offset+1);
			offset = strcspn(nextStart, " \t");
		}
		argv[i+1] = 0;
	}
	*argvp = argv;
	return argn;
}
void freemakeargv(int argn, char ** argv) {
	int i;
	for (i = 0; i < argn; i++)
		free(argv[i]);
	free(argv);
}

#ifdef DEBUG_makeargv
const char * argn_error = "Less than two argument";
const char * argv_failed = "Makeargv failed!";
const char * arg_num = "The argument array contains:\n";
int main() {
	int i;
	char **myargv;
	char *argv = "This is a command!";
	int numtokens = makeargv(argv, &myargv);
	if (numtokens == -1) {
		write(STDERR, argv_failed, strlen(argv_failed));
		return -1; 
	}
	write(STDOUT, arg_num, strlen(arg_num));
	for (i = 0; i < numtokens; i++)
		write(STDOUT, myargv[i], strlen(myargv[i]));
	freemakeargv(numtokens, myargv);
	return 0;
}
#endif
