#ifndef _FCMD_H_
#define _FCMD_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mxx_error_code.h"

#define PRINTF  printf
#define PUTC	putchar

#ifdef __cplusplus
extern "C" {
#endif

void fcmd_exec(uint8_t *cmd);

#ifdef __cplusplus
}
#endif

#endif
