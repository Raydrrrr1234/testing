/* listlib.h--
   AUTHOR:Ruikang Dai
   DATE:Sat Mar 14 11:55:21 EDT 2015
*/
#include <time.h>
#include <stdio.h>
typedef struct data_struct {
     time_t time;
     char *string;
} data_t;

int accessdata(void);
int adddata(data_t data);
int freekey(int key);
int getdata(int key, data_t *datap);
int runproc(char *cmd);
void showhistory(FILE *f);
