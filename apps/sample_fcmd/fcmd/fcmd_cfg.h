#ifndef _FCMD_CFG_H_
#define _FCMD_CFG_H_
/*******************************************************************************
* 用户函数命令头文件包含，函数声明
*/

#include "cmd_mem.h"


int get_test(char *server, int port, char *url);
int sslget_test(char *server, uint16_t port, char *url);

/**
 * 系统命令表
 */
CmdTbl_t CmdSysTbl[] =
{
	{"l", (void(*)(void))sys_ls},
	{"h", (void(*)(void))sys_h},
	{"q", (void(*)(void))sys_q},
};
/**
 * 函数命令表
 */
CmdTbl_t CmdTbl[] =
{
	{"void md(int addr, int elem_cnt, int elem_size)", (void(*)(void))md},
	{"int cmp(void *addr1, void *addr2, int elem_cnt, int elem_size)", (void(*)(void))cmp},
	{"void memset(void *s, int c, size_t n)", (void(*)(void))memset},
	{"void *memcpy(void *s1, const void *s2, size_t n)", (void(*)(void))memcpy},
	{"void *malloc(size_t size)", (void(*)(void))malloc},
	{"void free(void *ptr)", (void(*)(void))free},
	{"long strtol(const char *nptr, char **endptr, int base)", (void(*)(void))strtol},
	{"char *strstr(const char *s1, const char *s2)", (void(*)(void))strstr},
	{"FILE *fopen(const char *file,const char *mode)", (void(*)(void))fopen},
	{"int fclose(File *fp)", (void(*)(void))fclose},
	{"size_t fwrite(const void *p,size_t size,size_t nmemb, FILE *fp)", (void(*)(void))fwrite},
	{"size_t fread(void *p, size_t size, size_t nmenb, FILE *fp)", (void(*)(void))fread},
	{"int strncmp(char *s1,char *s2,int n)",	(void(*)(void))strncmp},
	{"int printf(char *fmt,...)",	(void(*)(void))printf},

	{"int get_test(char *server, uint16_t port, char *url)", (void(*)(void))get_test},
	{"int sslget_test(char *server, uint16_t port, char *url)", (void(*)(void))sslget_test},

};
uint8_t CmdSysTblSize = sizeof(CmdSysTbl) / sizeof(CmdSysTbl[0]);
uint8_t CmdTblSize = sizeof(CmdTbl) / sizeof(CmdTbl[0]);

#endif
