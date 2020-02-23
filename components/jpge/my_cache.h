#ifndef _MY_CACHE_H_
#define _MY_CACHE_H_

#include <stdbool.h>
#include "kernel/os/os.h"
#include "fb_io.h"

// cache_lseek
enum
{
	CACHE_SEEK_SET = 0,
	CACHE_SEEK_CUR,
	CACHE_SEEK_END,
};
enum
{
	CACHE_WR = 0,
	CACHE_RD,
};

// cache_io.flag
#define CACHE_FLAG_DMA				(1<<0)
#define CACHE_FLAG_FIFOA			(1<<1)
#define CACHE_FLAG_FIFOB			(1<<2)
#define CACHE_FLAG_FRAME			(1<<3)
#define CACHE_FLAG_FIFO_OVERFLOW	(1<<4)
#define CACHE_FLAG_FB_OVERFLOW		(1<<5)

#define CACHE_MAGIC		0xC0DE9798
struct cache_io;

struct cache_io
{
	// cache文件
	uint32_t addr;	// cache addr in flash
	uint32_t magic;
	int max_size;
	int size;
	int wr;			// write f_pos
	int rd;			// read f_pos
	int ref_cnt;	// current cache open cnt

	// cache_loop线程
	struct fb_io *pfb;
	uint8_t flag;
	char name[16];
	uint32_t t0, t1, t2;
	void (*hook)(struct cache_io *c, uint8_t evt_bit);
	OS_Semaphore_t start_sem;
	OS_Semaphore_t fb_sem;		// fb_io接收fifo_a/fifo_b成功
	OS_Semaphore_t frame_sem;	// fb_io接收一帧完成
	OS_Semaphore_t cache_sem;	// fb_io全部写入cache完成
	OS_Thread_t cache_thread;
};

int cache_init(struct cache_io *c, void (*func)(void *), struct fb_io *fb);
int cache_exit(struct cache_io *c);
void cache_hook(struct cache_io *c, uint8_t evt_bit);
void cache_loop(void *arg);
bool cache_write_fb(struct cache_io *c, uint8_t *buf, uint32_t n);
int cache_start_fb(struct cache_io *c);

int test_cache(uint8_t ms, int total_len);

int cache_config(struct cache_io *c, uint32_t addr, int max_size);
int cache_open(struct cache_io *c);
int cache_write(struct cache_io *c, uint8_t *buf, uint32_t n);
int cache_read(struct cache_io *c, uint8_t *buf, uint32_t n);
int cache_clear(struct cache_io *c);
int cache_lseek(struct cache_io *c, int wr_rd, int offset, int whence);
int cache_size(struct cache_io *c);
int cache_close(struct cache_io *c);

#endif
