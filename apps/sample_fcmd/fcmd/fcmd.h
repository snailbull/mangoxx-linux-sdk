#ifndef _FCMD_H_
#define _FCMD_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PRINTF(fmt, args...)		printf(fmt, ##args)

typedef struct
{
	char *fname;
	void (*pfunc)(void);
} CmdTbl_t;

void sys_ls(void);
void sys_h(void);
void sys_q(void);

void fcmd_exec(char *cmd);
void fcmd_register(CmdTbl_t *func_tbl, int func_tbl_size, CmdTbl_t *sys_tbl, int sys_tbl_size);

#ifdef __cplusplus
}
#endif

#endif
