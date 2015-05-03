#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#define CLUSTER 1024
#define CLUSTER_NUM 10000
#define FLAG 126
#define INDEXSIZE 32
#define INDEXNAMELEN 8
#define EXTENDLEN 3
#define CHAR_S 1

#define ZERO_C 128
#define MAX CLUSTER_NUM-ZERO_C

// Attribute macro
#define ATTR_DELETED 0x00000001

// create a file
// Order: char name [8], char extend [3], char attribute, char backup [2], char mtime_h_m_s [2], char mtime_y_m [2], char view_y_m [2]
// Char from_h [2], char modify_h_m_s [2], char modify_y_m [2], char from_l [2], char size [4]
int makeFile (int cluster_no, int cluster_no_to, char * FolderName, char * extend, char attribute, int index_no, int size);

// get the current time, hms represent time in minutes and seconds, ym is the date
void getTime (char * hms, char * ym);

// Set a disk block is occupied
int setNumUse (int num);

// format the entire disk
int formating ();

// initialize buff, clusterFlag, current_cluster, FATtable
void initial ();

// output all files and directories in the current directory
void fetch ();

// Get the name stored in the directory inside
void getMsgName (char * buff, char * name);

// create a folder (not used)
int makeFolder (int cluster_no);

// find free disk inside fast in bitMAP
int findFreeCluster ();

// find free index in the current directory
int findFreeIndex ();

// find folders by name
int find_Index (char * index);

// write data into the specified size size cluster_no
void writeMsg (unsigned int cluster_no, int size);
void writeMsg_Init (unsigned int cluster_no, char * ini);

// get the properties of an item from the catalog
char getMsgAttr (char * buff);

// get the extension of the file
void getMsgExtend (char * buff, char * extend);

// delete the files in the current directory XXX.xxx returns the corresponding point of the first block
int del_Index (char * name_in, char * extend_in);

// delete the index relative path
int del_Index_Soft (char * name_in, char * extend_in, int cur_cluster);

// Get the first block of the current directory pointing
int getMsgCluster (int i);

// relative path
int getMsgCluster_Soft (char * buff, int i);

// Set a disk block is empty, need - ZERO_C
int setNumVacant (int num);

// recursively delete a file, you need to give the first block of cluster
int del_File (unsigned short cluster);

// display bitmap
void showBMP ();

// display FAT table
void showFAT ();

// find the file in the current directory
int find_File (char * name_in, char * extend_in);

// split a file name and extension
int cut_Name (char * cm, char * name, char * extend);

// Copy file to another path
void copyFile (char * to_name, int next_cluster, int size);

// Allocate a free space from disk
void alloc_FileSpace (unsigned short next_cluster, int size);

// Calculate the size of file for "copyFile" function
int find_File_Size (char * name_in, char * extend_in);


int getMsgSize (int i);
int getMsgSize_Soft (char * buff, int i); // relative

// Delete a folder
int del_Folder (char * name_in, int cur_cluster);

// Delete all files
int del_All (int cluster_no);

// Delete a specified file
void delCurrentFile (char * name, char * extend, int cur_cluster);

// Get the parent path
void getParent (char * input);

FILE * stream;
// Storing file table in memory can improve efficiency
char FAT_IN_MEMORY [125 * CLUSTER]; // Fat file table is a blank disc logic + ZERO_C
char clusterFlag [2 * CLUSTER];
int current_cluster;
char buff [CLUSTER];