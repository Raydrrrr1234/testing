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
//数据0盘块和数据最大盘块
#define ZERO_C 128
#define MAX CLUSTER_NUM-ZERO_C

//属性宏
#define ATTR_DELETED 0x00000001

//顺序:char name[8],char extend[3],char attribute,char backup[2],char mtime_h_m_s[2],char mtime_y_m[2],char view_y_m[2]
//char from_h[2],char modify_h_m_s[2],char modify_y_m[2],char from_l[2],char size[4]
int makeFile(int cluster_no,int cluster_no_to,char *FolderName,char *extend,char attribute,int index_no,int size);//创建文件
void getTime(char *hms, char *ym);//获取当前时间，hms表示获取到的现在的时分秒，ym表示取到的年月日
int setNumUse(int num);//设置某盘块被占用
int formating();//格式化整个盘
void initial();//初始化buff,clusterFlag,current_cluster,FATtable
void fetch();//输出所有的当前目录的文件和目录
void getMsgName(char *buff,char *name);//获取存放在目录里面的名字
int makeFolder(int cluster_no);//创建文件夹（未使用）
int findFreeCluster();//在bitMAP里面找空闲盘快
int findFreeIndex();//在当前目录找空闲index
int find_Index(char *index);//根据名字找文件夹
void writeMsg(unsigned int cluster_no,int size);//写size大小的数据进入指定的cluster_no
void writeMsg_Init(unsigned int cluster_no,char * ini);
char getMsgAttr(char *buff);//从目录中获取该某项的属性
void getMsgExtend(char *buff,char *extend);//获取后缀名
int del_Index(char *name_in, char *extend_in);//删除当前目录中的文件XXX.xxx返回对应的指向的第一个块
int del_Index_Soft(char *name_in, char *extend_in, int cur_cluster);//相对路径删除index 
int getMsgCluster(int i);//获取当前目录中该项的指向的第一个块
int getMsgCluster_Soft(char *buff,int i); //相对路径
int setNumVacant(int num);//设置某盘块为空,需要 - ZERO_C
int del_File(unsigned short cluster);//递归删除某文件，需要给出第一个块的cluster
void showBMP();//显示bitmap
void showFAT();//显示FAT表
int find_File(char *name_in, char *extend_in);//当前目录查找文件
int cut_Name(char *cm, char* name, char *extend);//分割文件名和后缀 
void copyFile(char *to_name,int next_cluster,int size);
void alloc_FileSpace(unsigned short next_cluster,int size);
int find_File_Size(char *name_in, char *extend_in);
int getMsgSize(int i);
int getMsgSize_Soft(char *buff,int i);//相对
int del_Folder(char *name_in,int cur_cluster);
int del_All(int cluster_no);
void delCurrentFile(char *name,char *extend,int cur_cluster);
void getParent(char *input);

FILE *stream;
//内存中的表可以提高效率
char FAT_IN_MEMORY[125*CLUSTER];		//记录的是逻辑上的空盘+ZERO_C
char clusterFlag[2*CLUSTER];
int current_cluster;
char buff[CLUSTER];

