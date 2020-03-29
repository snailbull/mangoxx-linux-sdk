#ifndef __ESP_SPIFFS_H__
#define __ESP_SPIFFS_H__

#include "spiffs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FS1_FLASH_SIZE      (256*1024)
#define FS1_FLASH_ADDR      (0x00200000)
#define SECTOR_SIZE         (4096) 
#define LOG_BLOCK           (SECTOR_SIZE)
#define LOG_PAGE            (128)
#define FD_BUF_SIZE         32*4
#define CACHE_BUF_SIZE      (LOG_PAGE + 32)*8

struct esp_spiffs_config {
    uint32 phys_size;        /**< physical size of the SPI Flash */
    uint32 phys_addr;        /**< physical offset in spi flash used for spiffs, must be on block boundary */
    uint32 phys_erase_block; /**< physical size when erasing a block */

    uint32 log_block_size;   /**< logical size of a block, must be on physical block size boundary and must never be less than a physical block */
    uint32 log_page_size;    /**< logical size of a page, at least log_block_size/8  */

    uint32 fd_buf_size;      /**< file descriptor memory area size */
    uint32 cache_buf_size;   /**< cache buffer size */
};

s32_t esp_spiffs_init(struct esp_spiffs_config *config);

void esp_spiffs_deinit(uint8 format);

void myspiffs_init(void);


void list(char *path);
void cat(char *fname);
void import (char *fname, char *src, int srclen);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SPIFFS_H__ */
