/* qsort.c--
   AUTHOR:Ruikang Dai
   DATE:Sun Mar  1 14:42:36 EST 2015
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "restart.h"
#define MAXLINES 5000 
char *lineptr[MAXLINES]; 

void qsort(void *lineptr[], int left, int right, int (*comp)(void *, void *));
int numcmp(char *, char *);

// sort input lines
int main(int argn, char **argv) {
	int nlines;
	int numeric = 0;
	int fd;
	// This is the part that I am not familiar now!!!
	if((fd = open("./data", O_RDWR )) == -1 && errno == EINTR)
		perror("Failed to open the file, please create the sort list in ./data");
	if(argn > 1 && strcmp(argv[1], "-n") == 0)
		numeric = 0;
	if((nlines = readline(fd, lineptr, MAXLINES)) >= 0) {
		qsort((void**) lineptr, 0, nlines-1, (int (*)(void *, void *))(numeric ? numcmp : strcmp));
		writeline(fd, lineptr, nlines);
		return 0;
	}
	else {
		printf("input too big to sort\n");
		return 1;
	}
}

// qsort: sort v[left]...v[right] into increasing order
void qsort(void *v[], int left, int right, int (*comp)(void *, void *)) {
	int i, last;
	
	void swap (void *v[], int, int);
	
	if (left >= right)
		return;
	swap (v, left, (left +right)/2);
	last = left;
	for (i = left+1; i <= right; i++)
		if ((*comp) (v[i], v[left]) < 0)
			swap(v, ++last, i);
	swap(v, left, last);
	qsort(v, left, last-1, comp);
	qsort(v, last+1, right, comp);
}
int numcmp(char *s1, char *s2) {
	double v1, v2;
	
	v1 = fscanf (s1, "%lf", &v1);
	v2 = fscanf (s2, "%lf", &v2);
	if (v1 < v2)
		return -1;
	else if (v1 > v2)
		return 1;
	else
		return 0;
}
void swap(void *v[], int i, int j) {
	void *temp;

	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}
