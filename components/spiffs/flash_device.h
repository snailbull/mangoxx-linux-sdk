#ifndef _FLASH_DEVICE_H_
#define _FLASH_DEVICE_H_

#include <stdint.h>

int flash_init(char *fname);
int flash_exit(void);
int flash_flush(void);
int flash_read(uint32_t addr, uint32_t size, uint8_t *dst);
int flash_write(uint32_t addr, uint32_t size, uint8_t *src);
int flash_erase(uint32_t addr, uint32_t size);

#endif
