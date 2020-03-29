#ifndef _CMD_MEM_H_
#define _CMD_MEM_H_
#include <stdint.h>

void md(int addr, int elem_cnt, int elem_size);
int cmp(void *addr1, void *addr2, int elem_cnt, int elem_size);
int sfmd(uint32_t addr, int elem_cnt, int elem_size);
int sfmw(uint32_t writeaddr, uint8_t *pbuf, uint32_t num);
int sfmc(uint32_t erase_addr, uint32_t erase_mode, uint32_t num);
int sfmu(uint32_t start_addr, uint32_t sectors);

#endif
