#ifndef _FLASH_SPIFFS_H__
#define _FLASH_SPIFFS_H__
#include <stdint.h>
#include "spiffs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FS1_FLASH_SIZE      (512*1024)
#define FS1_FLASH_ADDR      (0x00200000)
#define SECTOR_SIZE         (4096) 
#define LOG_BLOCK           (SECTOR_SIZE)
#define LOG_PAGE            (128)
#define FD_BUF_SIZE         (32*4)
#define CACHE_BUF_SIZE      (LOG_PAGE + 32)*8

int platform_spiffs_init(void);
int platform_spiffs_exit(int format);
int test_list(char *path);
int test_cat(char *fname);
int test_import(char *fname, char *src, int srclen);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SPIFFS_H__ */
