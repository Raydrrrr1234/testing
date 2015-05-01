#include "FS.h" //自己的头文件
int main( void )
{
	int flag,len;
	unsigned int size;
	char name[INDEXNAMELEN+1];
	char extend[EXTENDLEN+1];
	char cm[256];
	char path[1025];

	int free_cluster,free_index;
	stream = fopen( "SDisk.dat", "r+b");
	if( stream == NULL )//打不开文件
	{
		fprintf( stdout, "磁盘不存在，重新生成\n" );
		stream = fopen( "SDisk.dat", "wb+");
		printf("正在格式化，请稍等...\n");
		formating();
		printf("格式化完毕！\n");
		
	}
	fclose(stream);
	stream = fopen( "SDisk.dat", "r+b");
	initial();
	sprintf(path,"%s",">ROOT");
	while(1)
	{
		printf("\n");
		printf("%s#",path);
		scanf("%s",cm);

		if(!strcmp(cm,"exit"))
		{
			fseek(stream,CLUSTER*(ZERO_C+current_cluster),SEEK_SET);
			fwrite(buff,CHAR_S,CLUSTER,stream);				//必须为read，因为修改了多个cluster
			fseek(stream,CLUSTER*FLAG,SEEK_SET);
			fwrite(clusterFlag, CHAR_S, 2*CLUSTER, stream);
			break;
		}
		else
		{
			if(!strcmp(cm,"dir"))
			{
				fetch();
			}
			else if(!strcmp(cm,"md"))
			{
				memset(name,0,CHAR_S*(INDEXNAMELEN+1));
				scanf("%s",cm);
				if(strlen(cm)>8)
				{
					printf("文件名太长了，将缩短文件名\n");
					memcpy(name,cm,7*CHAR_S);
					name[7] = '~';
				}
				else
					memcpy(name,cm,strlen(cm)*CHAR_S);
				free_cluster = findFreeCluster();
				free_index = findFreeIndex();
				if(free_index == -1 || free_cluster == -1)
				{
					printf("Sorry,本目录下最多只能存30个文件或者空间不足了\n");
				}
				else
				{
					setNumUse(ZERO_C+free_cluster);
					makeFile(ZERO_C+current_cluster,ZERO_C+free_cluster,name,"",0,free_index,0);
					makeFile(ZERO_C+free_cluster,ZERO_C+free_cluster,".","",0,0,0);
					makeFile(ZERO_C+free_cluster,ZERO_C+current_cluster,"..","",0,1,0);
				}

				//更新当前buff
				fseek(stream,CLUSTER*(ZERO_C+current_cluster),SEEK_SET);
				fread(buff,CHAR_S,CLUSTER,stream);				//必须为read，因为修改了多个cluster
				fseek(stream,CLUSTER*FLAG,SEEK_SET);
				fwrite(clusterFlag, CHAR_S, 2*CLUSTER, stream);
			}
			else if(!strcmp(cm,"rd"))
			{
				scanf("%s",cm);
				if(!del_Folder(cm,current_cluster+ZERO_C))
				{
					printf("文件不存在\n");
				}
				else
					printf("删完了！！\n");
				fseek(stream,CLUSTER*(ZERO_C+current_cluster),SEEK_SET);
				fread(buff,CHAR_S,CLUSTER,stream);	
			}
			else if(!strcmp(cm,"cd"))
			{
				scanf("%s",cm);
				flag = find_Index(cm);
				if(!flag)
				{
					printf("文件\"%s\"不存在，请重新输入\n",cm);
					continue;
				}
				current_cluster = flag-ZERO_C;
				//更新当前buff
				fseek(stream,CLUSTER*(ZERO_C+current_cluster),SEEK_SET);
				fread(buff,CHAR_S,CLUSTER,stream);
				//更新路径path
				if(!strcmp(cm,".."))
				{
					getParent(path);
				}
				else
					sprintf(path,"%s/%s",path,cm);
			}
			else if(!strcmp(cm,"CreateFile") || !strcmp(cm,"cf"))
			{
				scanf("%s%d",cm,&size);
				if(!cut_Name(cm,name,extend))
					continue;
				if(strlen(extend)>3)
				{
					printf("非法后缀\n");
					continue;
				}
				free_cluster = findFreeCluster();
				free_index = findFreeIndex();
				if(free_index == -1 || free_cluster == -1)
				{
					printf("Sorry,本目录下最多只能存30个文件或者空间不足了\n");
				}
				else
				{
					setNumUse(ZERO_C+free_cluster);
					makeFile(ZERO_C+current_cluster,ZERO_C+free_cluster,name,extend,0,free_index,size);
				}
				fseek(stream,CLUSTER*(ZERO_C+current_cluster),SEEK_SET);
				fread(buff,CHAR_S,CLUSTER,stream);				//必须为read，因为修改了很多个cluster
				fseek(stream,CLUSTER*1,SEEK_SET);
				fwrite(FAT_IN_MEMORY,CHAR_S*2,CLUSTER*125/2,stream);
				fseek(stream,CLUSTER*FLAG,SEEK_SET);
				fwrite(clusterFlag, CHAR_S, 2*CLUSTER, stream);
			}
			else if(!strcmp(cm,"DelFile") || !strcmp(cm,"df"))
			{
				scanf("%s",cm);
				if(!cut_Name(cm,name,extend))
					continue;
				delCurrentFile(name,extend,current_cluster+ZERO_C);
				//更新当前
				fseek(stream,CLUSTER*(ZERO_C+current_cluster),SEEK_SET);
				fread(buff,CHAR_S,CLUSTER,stream);
				fseek(stream,CLUSTER*1,SEEK_SET);
				fwrite(FAT_IN_MEMORY,CHAR_S*2,CLUSTER*125/2,stream);
				fseek(stream,CLUSTER*FLAG,SEEK_SET);
				fwrite(clusterFlag, CHAR_S, 2*CLUSTER, stream);
			}
			else if(!strcmp(cm,"CopyFile")||!strcmp(cm,"cpf"))
			{
				scanf("%s",cm);
				if(!cut_Name(cm,name,extend))
					continue;
				flag = find_File(name,extend);
				if(!flag)
				{
					printf("文件\"%s\"不存在，请重新输入\n",cm);
					continue;
				}
				scanf("%s",cm);
				copyFile(cm,flag-ZERO_C,find_File_Size(name,extend));
			}
			else if(!strcmp(cm,"ShowBitMP")||!strcmp(cm,"sbp"))
			{
				showBMP();
			}
			else if(!strcmp(cm,"ShowFAT")||!strcmp(cm,"sft"))
			{
				showFAT();
			}
			else if(!strcmp(cm,"formatdisk")||!strcmp(cm,"fd"))
			{
				fclose(stream);
				stream = freopen( "SDisk.dat", "w+", stderr);
				printf("正在格式化，请稍等...\n");
				formating();
				printf("格式化完毕！\n");
				fclose(stream);
				stream = freopen( "SDisk.dat", "r+", stderr);
				initial();
				sprintf(path,"%s",">ROOT");
			}
			else
			{
				printf("不存在\"%s\"这样的命令\n",cm);
				fflush(stdin);
			}
		}
	}
	fclose( stream );
	return 0;
}
void delCurrentFile(char *name,char *extend,int cur_cluster)//这里删除就不能使用当前路径了，需要相对路径
{
	int flag;
	flag = del_Index_Soft(name,extend,cur_cluster);			//相对
	if(!flag)
	{
		printf("文件\"%s.%s\"不存在，请重新输入\n",name,extend);
		return;
	}
	del_File(flag-ZERO_C);								//绝对
}
int del_Folder(char *name_in,int cur_cluster)//这里删除就不能使用当前路径了，需要相对路径
{
	int i;
	char buff[CLUSTER];//相对
	char name[INDEXNAMELEN+1];
	char extend[EXTENDLEN+1];
	char attribute;
	int next_cluster;//相对
	fseek(stream,cur_cluster*CLUSTER,SEEK_SET);//相对
	fread(buff,CHAR_S,CLUSTER,stream);
	for(i = INDEXSIZE*2; i < CLUSTER;i+=INDEXSIZE)		//前面两个"." ".."文件夹不可以删
	{
		getMsgName(&buff[i],name);
		attribute = getMsgAttr(&buff[i]);
		if( strlen(name) > 0 && !( attribute & ATTR_DELETED ) && !strcmp(name,name_in) )
		{
			buff[i+11] |= ATTR_DELETED;
			next_cluster = getMsgCluster_Soft(buff,i);		//相对
			setNumVacant(next_cluster);					//绝对
			fseek(stream,cur_cluster*CLUSTER+i,SEEK_SET);//相对
			fwrite(&buff[i],CHAR_S,INDEXSIZE,stream);
			return del_All(next_cluster);				//相对
		}
	}
	
	return 0;
}
int del_All(int cur_cluster)//这里删除就不能使用当前路径了，需要相对路径
{
	int i;
	int flag;
	char buff[CLUSTER];									//相对
	char name[INDEXNAMELEN+1];
	char extend[EXTENDLEN+1];
	char attribute;	
	fseek(stream,cur_cluster*CLUSTER,SEEK_SET);			//相对
	fread(buff,CHAR_S,CLUSTER,stream);			
	for(i = INDEXSIZE*2; i < CLUSTER;i+=INDEXSIZE)		//前面两个"." ".."文件夹不可以删
	{
		getMsgName(&buff[i],name);
		attribute = getMsgAttr(&buff[i]);
		if( strlen(name) > 0 && !( attribute & ATTR_DELETED ) )
		{
			flag = getMsgSize_Soft(buff,i);					//相对
			if(flag > 0)
			{
				getMsgExtend(&buff[i+8],extend);
				delCurrentFile(name,extend,cur_cluster);
			}
			else
				del_Folder(name,cur_cluster);
		}
	}
	return 1;
}
int find_File(char *name_in, char *extend_in)
{
	int i;
	char name[INDEXNAMELEN+1];
	char extend[EXTENDLEN+1];
	char attribute;
	for(i = INDEXSIZE*2; i < CLUSTER;i+=INDEXSIZE)
	{
		getMsgName(&buff[i],name);
		attribute = getMsgAttr(&buff[i]);
		getMsgExtend(&buff[i+8],extend);
		if( strlen(name) > 0 && !( attribute & ATTR_DELETED ) && !strcmp(name,name_in) && !strcmp(extend,extend_in) )
		{
			return getMsgCluster(i);
		}
	}
	return 0;
}
void copyFile(char *to_name,int next_cluster,int size)
{
	char tmp[1024];
	char name[INDEXNAMELEN+1];
	char extend[EXTENDLEN+1];
	int free_cluster,free_index;
	if(!cut_Name(to_name,name,extend))
			return;
	free_cluster = findFreeCluster();
	free_index = findFreeIndex();
	if(free_index == -1 || free_cluster == -1)
	{
		printf("Sorry,本目录下最多只能存30个文件或者空间不足了\n");
	}
	else
	{
		setNumUse(ZERO_C+free_cluster);
		makeFile(ZERO_C+current_cluster,ZERO_C+free_cluster,name,extend,0,free_index,size);
	}
}
int cut_Name(char *cm, char* name, char *extend)
{
	int len;
	len = strlen(cm);
	memset(name,0,CHAR_S*(INDEXNAMELEN+1));
	memset(extend,0,CHAR_S*(EXTENDLEN+1));
	while(len--)
	{
		if(cm[len] == '.')
			break;
	}
	if(len > 8)
	{
		memcpy(name,cm,7*CHAR_S);
		name[7] = '~';
	}
	else if(len == -1)
	{
		printf("文件名\"%s\"错误，缺少后缀！\n",cm);
		return 0;
	}
	else
	{
		memcpy(name,cm,(len)*CHAR_S);
	}
	memcpy( extend, &cm[len+1],(strlen(cm)-len-1)*CHAR_S );
	return 1;
}
void showFAT()
{
	int i;
	for(i = 0; i < 2*CLUSTER; i+=2)															//显示FAT表大小
	{
		printf("%X ",*(short *)&FAT_IN_MEMORY[i]);
	}
}
void showBMP()
{
	int i;
	for(i = 0; i < 2*CLUSTER; i+=4)															//显示BMP表大小
	{
		printf("%X ",*(int *)&clusterFlag[i]);
	}
}
int del_File(unsigned short cluster)
{
	if(*(short *)&FAT_IN_MEMORY[cluster*2] == -1)
	{
		setNumVacant(cluster+ZERO_C);
		return 0;
	}
	else
	{
		del_File(*(unsigned short *)&FAT_IN_MEMORY[cluster*2]);
		setNumVacant(cluster+ZERO_C);
	}
	return 1;
}
int getMsgSize_Soft(char *buff,int i)
{
	return *(int *)(&buff[i+28]);
}
int getMsgSize(int i)
{
	return *(int *)(&buff[i+28]);
}
int find_File_Size(char *name_in, char *extend_in)
{
	int i;
	char name[INDEXNAMELEN+1];
	char extend[EXTENDLEN+1];
	char attribute;
	for(i = INDEXSIZE*2; i < CLUSTER;i+=INDEXSIZE)
	{
		getMsgName(&buff[i],name);
		attribute = getMsgAttr(&buff[i]);
		getMsgExtend(&buff[i+8],extend);
		if( strlen(name) > 0 && !( attribute & ATTR_DELETED ) && !strcmp(name,name_in) && !strcmp(extend,extend_in) )
		{
			return getMsgSize(i);
		}
	}
	return 0;
}
int del_Index_Soft(char *name_in, char *extend_in, int cur_cluster)	//相对
{
	int i;
	char buff[CLUSTER];
	char name[INDEXNAMELEN+1];
	char extend[EXTENDLEN+1];
	char attribute;
	fseek(stream,cur_cluster*CLUSTER,SEEK_SET);
	fread(buff,CHAR_S,CLUSTER,stream);
	for(i = INDEXSIZE*2; i < CLUSTER;i+=INDEXSIZE)
	{
		getMsgName(&buff[i],name);
		attribute = getMsgAttr(&buff[i]);
		getMsgExtend(&buff[i+8],extend);
		if( strlen(name) > 0 && !( attribute & ATTR_DELETED ) && !strcmp(name,name_in) && !strcmp(extend,extend_in) )
		{
			buff[i+11] |= ATTR_DELETED;
			fseek(stream,cur_cluster*CLUSTER+i,SEEK_SET);//相对
			fwrite(&buff[i],CHAR_S,INDEXSIZE,stream);
			return getMsgCluster_Soft(buff,i);
		}
	}
	
	return 0;
}
int del_Index(char *name_in, char *extend_in)
{
	int i;
	char name[INDEXNAMELEN+1];
	char extend[EXTENDLEN+1];
	char attribute;
	for(i = INDEXSIZE*2; i < CLUSTER;i+=INDEXSIZE)
	{
		getMsgName(&buff[i],name);
		attribute = getMsgAttr(&buff[i]);
		getMsgExtend(&buff[i+8],extend);
		if( strlen(name) > 0 && !( attribute & ATTR_DELETED ) && !strcmp(name,name_in) && !strcmp(extend,extend_in) )
		{
			buff[i+11] |= ATTR_DELETED;
			return getMsgCluster(i);
		}
	}
	return 0;
}
int find_Index(char *index)
{
	int i;
	char name[INDEXNAMELEN+1];
	char attribute;
	for(i = INDEXSIZE; i < CLUSTER;i+=INDEXSIZE)
	{
		getMsgName(&buff[i],name);
		attribute = getMsgAttr(&buff[i]);
		if(strlen(name) > 0 && !( attribute & ATTR_DELETED ) && !(strcmp(name,index)))
			return getMsgCluster(i);
	}
	return 0;
}
int setNumVacant(int num)
{
	int offset,bit;
	char ini;
	num-=ZERO_C;
	offset=num/8;
	bit=num%8;
	ini = 1;
	ini= ini << bit;
	clusterFlag[offset] &= ~ini;
	return 0;
}
void getParent(char *input)
{
	int i,len;
	len = strlen(input);
	for(i = len-1;i >= 0; i--)
	{
		if(input[i] == '/')
			break;
	}
	input[i] = 0;
}
int findFreeIndex()//必须从第三个index开始找
{
	int i;
	char attribute;
	char name[INDEXNAMELEN+1];
	for(i = INDEXSIZE*2; i < CLUSTER;i+=INDEXSIZE)
	{
		getMsgName(&buff[i],name);
		attribute = getMsgAttr(&buff[i]);
		if(strlen(name) == 0 || (attribute&ATTR_DELETED))
			return i/INDEXSIZE;
	}
	return -1;
}
int findFreeCluster()//返回逻辑上的第n个空盘，需要+ZERO_C
{
	int i,j;
	char bit;
	for(i = 0; i < CLUSTER*2; i++)
	{
		if((int)clusterFlag[i] < 255)
		{
			bit = clusterFlag[i];
			for(j = 0; j < 8; j++)
			{
				if(bit%2 == 0)
					return i*8+j;
				else
					bit = bit>> 1;
			}
		}
	}
	return -1;
}
void initial()
{
	memset(FAT_IN_MEMORY,0,sizeof(FAT_IN_MEMORY));
	memset(clusterFlag,0,sizeof(clusterFlag));
	fseek(stream,CLUSTER*1,SEEK_SET);
	fread(FAT_IN_MEMORY,CHAR_S*2,CLUSTER*125/2,stream);
	fseek(stream,CLUSTER*FLAG,SEEK_SET);
	fread(clusterFlag,CHAR_S,CLUSTER*2,stream);
	current_cluster = 0;
	fseek(stream,CLUSTER*(ZERO_C+current_cluster),SEEK_SET);
	fread(buff,CHAR_S,CLUSTER,stream);	
}
int getMsgCluster_Soft(char *buff,int i)	//相对
{
	char tmp[4];
	tmp[3]= (buff[i+20] << 24);
	tmp[2]= (buff[i+21] << 16);
	tmp[1]= (buff[i+26] << 8);
	tmp[0]= (buff[i+27]);
	return *(int *)tmp;
}
int getMsgCluster(int i)//位运算转换
{
	char tmp[4];
	tmp[3]= (buff[i+20] << 24);
	tmp[2]= (buff[i+21] << 16);
	tmp[1]= (buff[i+26] << 8);
	tmp[0]= (buff[i+27]);
	return *(int *)tmp;
}

void printspace(int a)
{
	int i;
	for(i = 0; i < a; i++)
		printf(" ");
}
void head()
{
	printf("文件名");
	printspace(20-6);
	printf("文件类型");
	printspace(20-7);
	printf("大小");
	printf("\n");
}
void fetch()
{
	int i;
	char name[INDEXNAMELEN+1];
	char extend[EXTENDLEN+1];
	char attribute = 0;
	head();
	for(i = INDEXSIZE; i < CLUSTER;i+=INDEXSIZE)
	{
		getMsgName(&buff[i],name);
		attribute = getMsgAttr(&buff[i]);
		if(strlen(name) > 0 && !( attribute & ATTR_DELETED ))
		{
			printf("%s",name);//输出格式dir
			getMsgExtend(&buff[i+8],extend);
			if(*(int *)(&buff[i+28]) > 0 && strlen(extend)>0)
			{
				printf(".%s",extend);
				printspace(40-strlen(name)-strlen(extend));
				printf("%d B\n",*(int *)(&buff[i+28]));
			}
			else
			{
				printspace(20-strlen(name));
				printf("<DIR>\n");
			}
		}
	}
	printf("\n");
}
void getMsgExtend(char *buff,char *extend)
{
	memset(extend,0,CHAR_S*(EXTENDLEN+1));
	memcpy(extend,buff,CHAR_S*EXTENDLEN);
}
void getMsgName(char *buff,char *name)
{
	memset(name,0,CHAR_S*(INDEXNAMELEN+1));
	memcpy(name,buff,CHAR_S*INDEXNAMELEN);
}
char getMsgAttr(char *buff)
{
	return buff[11];
}
int formating()
{
	int i;
	char *ini;
	//申请一个簇大小的内存空间
	ini = (char *)malloc(CLUSTER);
	memset(ini,0,CLUSTER*CHAR_S);//全部0
	for(i = 0; i < CLUSTER_NUM; i++)
		fwrite( ini, sizeof( char ), CLUSTER, stream );//stream全局变量
	free(ini);
	//初始化第0个盘块
	fseek(stream,0,SEEK_SET);//fseek是按字节移动，移动光标
	ini = "FAT32 1024 10240 戴瑞康 2012/5/22";
	fwrite( ini, CHAR_S, strlen(ini) , stream);
	initial();
	setNumUse(ZERO_C);
	makeFile(ZERO_C,ZERO_C,".","",0,0,0);
	fseek(stream,CLUSTER*FLAG,SEEK_SET);
	fwrite(clusterFlag, CHAR_S, 2*CLUSTER, stream);

	return 0;
}

void getTime(char *hms, char *ym)
{
	time_t t = time(0);
	struct tm * tms= localtime(&t);
	hms[0] = (tms->tm_hour<<3)|(tms->tm_min>>3);
	hms[1] = (tms->tm_min<<5)|(tms->tm_sec);
	ym[0] = (tms->tm_year<<1)|(tms->tm_mon>>3);
	ym[1] = (tms->tm_mon<<5)|(tms->tm_mday);
}
//构造文件目录麻烦
//顺序:char name[8],char extend[3],char attribute,char backup[2],char mtime_h_m_s[2],char mtime_y_m[2],char view_y_m[2]
//char from_h[2],char modify_h_m_s[2],char modify_y_m[2],char from_l[2],char size[4]
int makeFile(int cluster_no,int cluster_no_to,char *FolderName,char *extend,char attribute,int index_no,int size)
{
	 
	char buff_t[32];
	memset(buff_t,0,32*CHAR_S);
	memcpy(buff_t,FolderName,CHAR_S*INDEXNAMELEN);
	memcpy(&buff_t[8],extend,CHAR_S*EXTENDLEN);
	buff_t[11] = attribute;
	getTime(&buff_t[14],&buff_t[16]);
	buff_t[18] = buff_t[16];
	buff_t[19] = buff_t[17];
	buff_t[20] = cluster_no_to >> 24;
	buff_t[21] = (cluster_no_to >> 16)%256;
	getTime(&buff_t[22],&buff_t[24]);
	buff_t[26] = (cluster_no_to % 65536)>>8;
	buff_t[27] = cluster_no_to % 256;
	*(int *)(&buff_t[28]) = size;
	fseek(stream,cluster_no*CLUSTER+index_no*INDEXSIZE,SEEK_SET);
	fwrite(buff_t,CHAR_S,INDEXSIZE,stream);
	
	if(size<CLUSTER*CLUSTER_NUM && size>0)
	{
		alloc_FileSpace((unsigned short)cluster_no_to - ZERO_C,size);
	}
	return 0;
}
void alloc_FileSpace(unsigned short next_cluster,int size)
{
	if(size-CLUSTER < 0)
		writeMsg(next_cluster+ZERO_C,size);
	else
		writeMsg(next_cluster+ZERO_C,CLUSTER);
	size -= CLUSTER;
	while(size > 0)
	{
		*(unsigned short *)&FAT_IN_MEMORY[next_cluster*2] = findFreeCluster();
		if((int)*(unsigned short *)&FAT_IN_MEMORY[next_cluster*2] == -1)
		{
			printf("空间不足，无法创建文件\n");
			fseek(stream,CLUSTER,SEEK_SET);
			fread(FAT_IN_MEMORY,CHAR_S*2,CLUSTER*125/2,stream);
			fseek(stream,CLUSTER*FLAG,SEEK_SET);
			fread(clusterFlag,CHAR_S,CLUSTER*2,stream);
			return;
		}
		next_cluster = *(unsigned short *)&FAT_IN_MEMORY[next_cluster*2];
		setNumUse(ZERO_C+next_cluster);
		if(size-CLUSTER < 0)
			writeMsg(next_cluster+ZERO_C,size);
		else
			writeMsg(next_cluster+ZERO_C,CLUSTER);
		size -= CLUSTER;
	}
	*(short *)&FAT_IN_MEMORY[next_cluster*2] = -1;
}
void writeMsg(unsigned int cluster_no,int size)
{
	char *ini;
	//申请一个size大小的内存空间
	ini = (char *)malloc(size);
	fseek(stream,cluster_no*CLUSTER,SEEK_SET);
	fwrite( ini, CHAR_S, size, stream);
	free(ini);
}
void writeMsg_Init(unsigned int cluster_no,char *ini)
{
	fseek(stream,cluster_no*CLUSTER,SEEK_SET);
	fwrite( ini, CHAR_S, CLUSTER, stream);
}
int setNumUse(int num)
{
	int offset,bit;
	char ini;
	num-=ZERO_C;
	offset=num/8;
	bit=num%8;
	ini = 1;
	ini= ini << bit;
	clusterFlag[offset] |= ini;
	return 0;
}
