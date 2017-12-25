#ifndef INIT_H
#define INIT_H

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BLOCKSIZE 1024
#define SIZE 1024000
#define END 65535
#define FREE 0
#define ROOTBLOCKNUM 2
#define MAXOPENFILE 10 // 同时打开最大文件数
#define MAX_TEXT_SIZE 10000

typedef struct FCB {
    char filename[8];
    char exname[3];
    unsigned char attribute; // 0: dir file, 1: data file
    unsigned short time;
    unsigned short date;
    unsigned short first;
    unsigned short length;
    char free; // 0: 空 fcb
} fcb;

typedef struct FAT {
    unsigned short id;
} fat;

typedef struct USEROPEN {
    char filename[8];
    char exname[3];
    unsigned char attribute;
    unsigned short time;
    unsigned short date;
    unsigned short first;
    unsigned short length;
    char free;

    int dirno; // 父目录文件起始盘块号
    int diroff; // 该文件对应的 fcb 在父目录中的逻辑序号
    char dir[MAXOPENFILE][80]; // 全路径信息
    int count;
    char fcbstate; // 是否修改 1是 0否
    char topenfile; // 0: 空 openfile
} useropen;

typedef struct BLOCK {
    char magic_number[8];
    char information[200];
    unsigned short root;
    unsigned char* startblock;
} block0;

unsigned char* myvhard;
//useropen *ptrcurdir;
useropen openfilelist[MAXOPENFILE];
int currfd;
//char currentdir[80];
unsigned char* startp;
//extern char *FILENAME;
unsigned char buffer[SIZE];

/* sysfile func */
void startsys();
void my_format();
void my_cd(char* dirname);
int do_read(int fd, int len, char* text);
int do_write(int fd, char* text, int len, char wstyle);
int my_read(int fd);
int my_write(int fd);

void exitsys();
void my_cd(char* dirname);
int my_open(char* filename);
int my_close(int fd);
void my_mkdir(char* dirname);
void my_rmdir(char* dirname);
int my_create(char* filename);
void my_rm(char* filename);
void my_ls();
void help();

int get_free_openfilelist();
unsigned short int get_free_block();

#endif
