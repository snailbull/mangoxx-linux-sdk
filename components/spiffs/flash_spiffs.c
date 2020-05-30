#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "spiffs.h"
#include "flash_spiffs.h"

#define NUM_SYS_FD 3
#define FLASH_UNIT_SIZE 4

struct flash_fs
{
	spiffs fs;
	uint8_t *work_buf;
	uint8_t *fd_buf;
	uint8_t *cache_buf;
};
static struct flash_fs fs_ctx;

static int lowlevel_spiffs_readwrite(uint32_t addr, uint32_t size, uint8_t *p, int write)
{
    /*
     * With proper configurarion spiffs never reads or writes more than
     * LOG_PAGE_SIZE
     */
    if (size > fs_ctx.fs.cfg.log_page_size) {
        printf("Invalid size provided to read/write (%d)\n\r", (int) size);
        return SPIFFS_ERR_NOT_CONFIGURED;
    }

    char tmp_buf[fs_ctx.fs.cfg.log_page_size + FLASH_UNIT_SIZE * 2];
    uint32_t aligned_addr = addr & (-FLASH_UNIT_SIZE);
    uint32_t aligned_size =
        ((size + (FLASH_UNIT_SIZE - 1)) & -FLASH_UNIT_SIZE) + FLASH_UNIT_SIZE;

    int res = flash_read((uint32_t)aligned_addr, aligned_size, (uint8_t *) tmp_buf);
    if (res != 0) {
        printf("flash_read failed: %d (%d, %d)\n\r", res, (int) aligned_addr,
               (int) aligned_size);
        return res;
    }

    if (!write) {
        memcpy(p, tmp_buf + (addr - aligned_addr), size);
        return SPIFFS_OK;
    }

    memcpy(tmp_buf + (addr - aligned_addr), p, size);
    res = flash_write(aligned_addr, aligned_size, (uint8_t *)tmp_buf);
    if (res != 0) {
	    printf("flash_write failed: %d (%d, %d)\n\r", res,
	              (int) aligned_addr, (int) aligned_size);
        return res;
    }

    return SPIFFS_OK;
}

static int _spiffs_read(uint32_t addr, uint32_t size, uint8_t *dst)
{
    return lowlevel_spiffs_readwrite(addr, size, dst, 0);
}
static int _spiffs_write(uint32_t addr, uint32_t size, uint8_t *src)
{
    return lowlevel_spiffs_readwrite(addr, size, src, 1);
}
static int _spiffs_erase(uint32_t addr, uint32_t size)
{
    if (size != fs_ctx.fs.cfg.phys_erase_block || addr % fs_ctx.fs.cfg.phys_erase_block != 0) {
        printf("Invalid size provided to _spiffs_erase (%d, %d)\n\r", (int) addr, (int) size);
        return SPIFFS_ERR_NOT_CONFIGURED;
    }

    return flash_erase(addr, fs_ctx.fs.cfg.phys_erase_block);
}

int platform_spiffs_init(void)
{
    if (SPIFFS_mounted(&fs_ctx.fs)) {
		printf("spiffs init err!\r\n");
        return -1;
    }

    spiffs_config cfg;
    int ret;

    cfg.phys_size = FS1_FLASH_SIZE;
    cfg.phys_addr = FS1_FLASH_ADDR;
    cfg.phys_erase_block = SECTOR_SIZE;
    cfg.log_block_size = LOG_BLOCK;
    cfg.log_page_size = LOG_PAGE;

    cfg.hal_read_f = _spiffs_read;
    cfg.hal_write_f = _spiffs_write;
    cfg.hal_erase_f = _spiffs_erase;

    if (fs_ctx.work_buf != NULL) {
        free(fs_ctx.work_buf);
        fs_ctx.work_buf = NULL;
    }
    fs_ctx.work_buf = (uint8_t*)malloc(cfg.log_page_size * 2);
    if (fs_ctx.work_buf == NULL) {
		printf("spiffs init err!\r\n");
        return -1;
    }

    if (fs_ctx.fd_buf != NULL) {
        free(fs_ctx.fd_buf);
        fs_ctx.fd_buf = NULL;
    }
    fs_ctx.fd_buf = (uint8_t*)malloc(FD_BUF_SIZE);
    if (fs_ctx.fd_buf == NULL) {
        free(fs_ctx.work_buf);
		printf("spiffs init err!\r\n");
        return -1;
    }

    if (fs_ctx.cache_buf != NULL) {
        free(fs_ctx.cache_buf);
        fs_ctx.cache_buf = NULL;
    }
    fs_ctx.cache_buf = (uint8_t*)malloc(CACHE_BUF_SIZE);
    if (fs_ctx.cache_buf == NULL) {
        free(fs_ctx.work_buf);
        free(fs_ctx.fd_buf);
		printf("spiffs init err!\r\n");
        return -1;
    }

    ret = SPIFFS_mount(&fs_ctx.fs, &cfg, fs_ctx.work_buf,
                        fs_ctx.fd_buf, FD_BUF_SIZE,
                        fs_ctx.cache_buf, CACHE_BUF_SIZE, 0);
    if (ret == -1) {
        free(fs_ctx.work_buf);
        free(fs_ctx.fd_buf);
        free(fs_ctx.cache_buf);
		printf("spiffs init err!\r\n");
		return -1;
    }
	printf("spiffs init ok!\r\n");
    return ret;
}

int platform_spiffs_exit(int format)
{
    if (SPIFFS_mounted(&fs_ctx.fs)) {
        SPIFFS_unmount(&fs_ctx.fs);
        free(fs_ctx.work_buf);
        free(fs_ctx.fd_buf);
        free(fs_ctx.cache_buf);
    }
	if (format) {
		SPIFFS_format(&fs_ctx.fs);
	}
	return 0;
}

static void die(char *what)
{
    printf("die:%s\r\n", what);
}

int test_list(char *path)
{
    spiffs_DIR dir;
    if (!SPIFFS_opendir (&fs_ctx.fs, path, &dir))
    {
        die("spiffs_opendir");
        return fs_ctx.fs.err_code;
    }
    struct spiffs_dirent de;
    while (SPIFFS_readdir (&dir, &de))
    {
        static const char types[] = "?fdhs"; // file, dir, hardlink, softlink
        char name[sizeof(de.name)+1] = { 0 };
        memcpy (name, de.name, sizeof(de.name));
        printf("%c %6u %s\n", types[de.type], de.size, name);
    }
    SPIFFS_closedir (&dir);
	return fs_ctx.fs.err_code;
}

int test_cat(char *fname)
{
    spiffs_file fh = SPIFFS_open (&fs_ctx.fs, fname, SPIFFS_RDONLY, 0);
    char *buf;
    buf = (char*)malloc(1024);
    if (buf == NULL)
    {
        printf("malloc err\n");
		return fs_ctx.fs.err_code;
    }

	printf("\n");
    int n;
	int i;
    while ((n = SPIFFS_read (&fs_ctx.fs, fh, buf, 1024)) > 0)
    {
		for (i=0; i<n; i++)
        	printf("%c", buf[i]);
    }
    SPIFFS_close (&fs_ctx.fs, fh);
    free(buf);
    buf = NULL;
	return fs_ctx.fs.err_code;
}

int test_import(char *fname, char *src, int srclen)
{
    spiffs_file fh = SPIFFS_open (&fs_ctx.fs, fname, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_WRONLY, 0);
    if (fh < 0)
    {
        printf("spiffs_open err\n");
		return fs_ctx.fs.err_code;
    }

    if (SPIFFS_write (&fs_ctx.fs, fh, src, srclen) < 0)
        die ("spiffs_write");

    SPIFFS_close (&fs_ctx.fs, fh);
	return fs_ctx.fs.err_code;
}
