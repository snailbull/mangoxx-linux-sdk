#ifndef _CMD_ALL_H_
#define _CMD_ALL_H_
#include "fcmd/fcmd.h"

#ifdef __cplusplus
extern "C" {
#endif

extern CmdTbl_t CmdTbl[];
extern CmdTbl_t CmdSysTbl[];
extern int CmdSysTblSize;
extern int CmdTblSize;

#ifdef __cplusplus
}
#endif

#endif
