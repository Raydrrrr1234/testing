/* getpaths.c--
	A program to test makeargv
   AUTHOR:Ruikang Dai
   DATE:Sat Mar 14 13:43:21 EDT 2015
*/
#include <stdio.h>
#define PATH_DELIMITERS ":"
int makeargv(const char *s, const char *delimiters, char ***argvp);

int main(int argn, char **argv) {
	char **myargv;
	char *path;

	path = getenv("PATH");
	if (makeargv(path, PATH_DELIMITERS, &myargv) == -1)
		return NULL;
	else
		return myargv;
}
