#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "driver/chip/hal_flash.h"
#include "my_cache.h"


/******************************************************************************
 * data cache in flash
 *
 * [0-3]: size
 * [4-...]: cache_data
 * data format: [15:0]:RRRRRGGG GGGBBBBB, little endian
 *   byte0 byte1  byte2 byte3 ......
 *   [7:0],[15:8],[7:0],[15:8]......
 */
static int s_cache_cnt;	// all cache open cnt

/**
 * 设置cache基地址, 最大size
 */
int cache_config(struct cache_io *c, uint32_t addr, int max_size)
{
	if (c->ref_cnt > 0)
	{
		return OS_FAIL;
	}
	c->addr = addr;
	c->max_size = max_size;
	return OS_OK;
}

/**
 * cache file io
 */
int cache_open(struct cache_io *c)
{
	if (c->ref_cnt > 0)
	{
		return OS_OK;
	}

	if (s_cache_cnt <= 0)
	{
		if (HAL_Flash_Open(0, HAL_WAIT_FOREVER) != HAL_OK)
		{
			printf("open flash(0) fail, cache_open error\n");
			return OS_FAIL;
		}
	}
	s_cache_cnt++;

	if (HAL_OK != HAL_Flash_Read(0, c->addr, (uint8_t *)&c->magic, 4))
	{
		printf("read flash(0) fail, c->size error\n");
		HAL_Flash_Close(0);
		return OS_FAIL;
	}
	if (HAL_OK != HAL_Flash_Read(0, c->addr+4, (uint8_t *)&c->size, 4))
	{
		printf("read flash(0) fail, c->size error\n");
		HAL_Flash_Close(0);
		return OS_FAIL;
	}
	c->ref_cnt++;
	if ((c->magic != CACHE_MAGIC) || ((c->size < 0) && (c->size > c->max_size)))
	{
		c->size = 0;
		cache_clear(c);
	}
	c->rd = 0;
	c->wr = 0;
	return OS_OK;
}
int cache_write(struct cache_io *c, uint8_t *buf, uint32_t n)
{
	if (c->ref_cnt <= 0)
	{
		printf("%s,cache_io.ref_cnt is <=0!\n", c->name);
		return OS_FAIL;
	}
	if ((n == 0) || ((c->size + n) > c->max_size))
	{
		return 0;
	}

	if (HAL_OK != HAL_Flash_Write(0, c->addr + 8 + c->wr, buf, n))
	{
		return OS_FAIL;
	}

	c->wr += n;
	c->size += n;

	return n;
}
int cache_read(struct cache_io *c, uint8_t *buf, uint32_t n)
{
	uint32_t len;

	if (c->ref_cnt <= 0)
	{
		printf("%s,cache_io.ref_cnt is <=0!\n", c->name);
		return OS_FAIL;
	}

	if (n == 0)
	{
		return 0;
	}

	if (c->rd >= c->size)
	{
		return 0;
	}
	else if ((c->rd + n) >= c->size)
	{
		len = c->size - c->rd;
	}
	else
	{
		len = n;
	}

	if (HAL_OK != HAL_Flash_Read(0, c->addr + 8 + c->rd, buf, len))
	{
		return OS_FAIL;
	}

	c->rd += len;
	return len;
}

/**
 * clear all cache
 */
int cache_clear(struct cache_io *c)
{
	int blocks;
	if (c->ref_cnt <= 0)
	{
		// printf("%s,cache_io.ref_cnt is <=0!\n", c->name);
		return OS_FAIL;
	}

	blocks = c->size / FLASH_ERASE_64KB + 1;	// align up
	printf("erase addr %08x, blocks %d, len %d.\r\n", c->addr, blocks, c->size);

	if (HAL_OK == HAL_Flash_Erase(0, FLASH_ERASE_64KB, c->addr, blocks))
	{
		printf("clear done!\r\n");
		uint32_t t = CACHE_MAGIC;
		if (HAL_OK != HAL_Flash_Write(0, c->addr, (uint8_t *)&t, 4))
		{
			printf("write c->magic error!\r\n");
			return OS_FAIL;
		}
		t = 0;
		if (HAL_OK != HAL_Flash_Write(0, c->addr+4, (uint8_t *)&t, 4))
		{
			printf("write c->size error!\r\n");
			return OS_FAIL;
		}
		c->magic = CACHE_MAGIC;
		c->size = 0;
		c->rd = 0;
		c->wr = 0;
	}
	else
	{
		printf("clear error!\r\n");
	}

	return OS_OK;
}

/**
 * move write/read pos
 *
 * @*c		cache_io body
 * @wr_rd	0:wr  1:rd
 * @offset	pos
 * @whence	SET,CUR,END(0,1,2)
 *
 * return   write/read pos
 *
 * note   wr fp pos will modify cache_io.size!
 */
int cache_lseek(struct cache_io *c, int wr_rd, int offset, int whence)
{
	if (c->ref_cnt <= 0)
	{
		printf("cache_io.ref_cnt is <=0!\n");
		return OS_FAIL;
	}
	if (wr_rd == CACHE_WR)
	{
		if (whence == CACHE_SEEK_SET)
		{
			if (offset >= c->max_size)
				offset = c->max_size;
			c->wr = offset, c->size = c->wr;
		}
		else if (whence == CACHE_SEEK_CUR)
		{
			if (c->wr + offset >= c->max_size)
				offset = c->max_size - c->wr;
			c->wr += offset, c->size = c->wr;
		}
		else if (whence == CACHE_SEEK_END)
		{
			c->wr = c->size;
		}
		else
		{
			printf("cache_lseek: whence err!\n");
			return OS_FAIL;
		}
		return c->wr;
	}
	else
	{
		if (whence == CACHE_SEEK_SET)
		{
			c->rd = offset;
		}
		else if (whence == CACHE_SEEK_CUR)
		{
			c->rd += offset;
		}
		else if (whence == CACHE_SEEK_END)
		{
			c->rd = c->size;
		}
		else
		{
			printf("cache_lseek: whence err!\n");
			return OS_FAIL;
		}
		return c->rd;
	}
}

int cache_size(struct cache_io *c)
{
	if (c->ref_cnt <= 0)
	{
		printf("cache_io.ref_cnt is <=0!\n");
		return OS_FAIL;
	}
	return c->size;
}
int cache_close(struct cache_io *c)
{
	if (c->ref_cnt <= 0)
	{
		return OS_OK;
	}

	uint8_t *buf = (uint8_t *)malloc(4096 + 8);
	uint8_t *pbuf = (uint8_t *)((((uint32_t)buf) + 3) & (~3));// align up

	// read
	int status = HAL_Flash_Read(0, c->addr, pbuf, 4096);
	if (status != HAL_OK)
	{
		printf("HAL_Flash_Read err!\n");
		free(buf);
		pbuf = buf = 0;
		return OS_FAIL;
	}

	// erase
	status = HAL_Flash_Erase(0, FLASH_ERASE_4KB, c->addr, 1);
	if (status != HAL_OK)
	{
		printf("HAL_Flash_Erase err!\n");
		free(buf);
		pbuf = buf = 0;
		return OS_FAIL;
	}

	// write
	pbuf[0] = CACHE_MAGIC & 0xFF;
	pbuf[1] = (CACHE_MAGIC >> 8) & 0xFF;
	pbuf[2] = (CACHE_MAGIC >> 16) & 0xFF;
	pbuf[3] = (CACHE_MAGIC >> 24) & 0xFF;
	pbuf[4] = c->size & 0xFF;
	pbuf[5] = (c->size >> 8) & 0xFF;
	pbuf[6] = (c->size >> 16) & 0xFF;
	pbuf[7] = (c->size >> 24) & 0xFF;
	status = HAL_Flash_Write(0, c->addr, pbuf, 4096);
	if (status != HAL_OK)
	{
		printf("HAL_Flash_Write err!\n");
		free(buf);
		pbuf = buf = 0;
		return OS_FAIL;
	}

	free(buf);
	pbuf = buf = 0;

	c->ref_cnt--;
	if (c->ref_cnt <= 0)
	{
		s_cache_cnt--;
		if (s_cache_cnt <= 0)
		{
			HAL_Flash_Close(0);
		}
	}

	return OS_OK;
}

/******************************************************************************
 * cache_loop thread&sem
 */
int cache_init(struct cache_io *c, void (*func)(void *), struct fb_io *fb)
{
	c->pfb = fb;
	c->flag = 0;
	c->hook = cache_hook;

	if (OS_OK != OS_SemaphoreCreate(&c->start_sem, 0, 1))
	{
		return OS_FAIL;
	}
	if (OS_OK != OS_SemaphoreCreate(&c->fb_sem, 0, 1))
	{
		OS_SemaphoreDelete(&c->start_sem);
		return OS_FAIL;
	}
	if (OS_OK != OS_SemaphoreCreate(&c->frame_sem, 0, 1))
	{
		OS_SemaphoreDelete(&c->start_sem);
		OS_SemaphoreDelete(&c->fb_sem);
		return OS_FAIL;
	}
	if (OS_OK != OS_SemaphoreCreate(&c->cache_sem, 0, 1))
	{
		OS_SemaphoreDelete(&c->start_sem);
		OS_SemaphoreDelete(&c->fb_sem);
		OS_SemaphoreDelete(&c->frame_sem);
		return OS_FAIL;
	}
	if (!OS_ThreadIsValid(&c->cache_thread))
	{
		OS_ThreadCreate(&c->cache_thread, c->name, func, c, OS_PRIORITY_NORMAL, 1024);
	}
	return OS_OK;
}

int cache_exit(struct cache_io *c)
{
	if (OS_ThreadIsValid(&c->cache_thread))
	{
		OS_ThreadDelete(&c->cache_thread);
	}
	if (OS_OK != OS_SemaphoreDelete(&c->start_sem))
	{
		printf("camera cache.start_sem err!\r\n");
	}
	if (OS_OK != OS_SemaphoreDelete(&c->fb_sem))
	{
		printf("camera cache.fb_sem err!\r\n");
	}
	if (OS_OK != OS_SemaphoreDelete(&c->frame_sem))
	{
		printf("camera cache.frame_sem err!\r\n");
	}
	if (OS_OK != OS_SemaphoreDelete(&c->cache_sem))
	{
		printf("camera cache.frame_sem err!\r\n");
	}
	return OS_OK;
}

/**
 * ImgSensor irq & DMA_IRQHandler transfer success hook!
 * @evt_bit  bit0:DMA_IRQHandler fifo_a/fifo_b transfer success
 *           bit1:CSI_IRQHandler fifo_a
 *           bit2:CSI_IRQHandler fifo_b
 *           bit3:CSI_IRQHandler frame_done
 *           bit4:CSI_IRQHandler FIFO_0_OVERFLOW!
 *           bit5:fb_io overflow: inc_head/inc_tail overflow
 *           bit6:cache_io is running!
 * @*arg
 *
 * note  Don's use printf in this function!!!
 *   evt_bit=5的时候表示fb_io内存不足，但是CSI接口还是会继续运行，直到CSI_FRAME_DONE_IRQ为止,
 *   CSI fifo_a/fifo_b会在同一个内存地址继续做DMA传输，fb_io.frame_size也会继续累加,
 *   fb_io.head指针不会继续向前移动, fb_io.tail正常工作.
 */
void cache_hook(struct cache_io *c, uint8_t evt_bit)
{
	c->flag |= evt_bit;
	if (evt_bit & CACHE_FLAG_DMA)
	{
		OS_SemaphoreRelease(&c->fb_sem);
		c->flag &= ~CACHE_FLAG_DMA;
	}
	else if (evt_bit & CACHE_FLAG_FRAME)
	{
		c->t1 = OS_GetTicks();
		OS_SemaphoreRelease(&c->frame_sem);
		c->flag &= ~CACHE_FLAG_FRAME;
	}
}

void cache_loop(void *arg)
{
	struct cache_io *c = (struct cache_io *)arg;
	int len, bw;
	while (1)
	{
		OS_SemaphoreWait(&(c->start_sem), OS_WAIT_FOREVER);
		for (;;)
		{
			if ((OS_OK == OS_SemaphoreWait(&(c->fb_sem), 100)) || fb_io_data_total_len(c->pfb))
			{
				len = fb_io_data_len(c->pfb);
				if (len > 0)
				{
					bw = cache_write(c, (uint8_t *)fb_io_data(c->pfb), len);
					if (bw > 0)
					{
						fb_io_inc_tail(c->pfb, bw);
					}
				}
			}

			if (c->pfb->frame_done && !fb_io_data_total_len(c->pfb))
			{
				c->t2 = OS_GetTicks();
				OS_SemaphoreRelease(&c->cache_sem);
				break;
			}

			if (c->flag & CACHE_FLAG_FB_OVERFLOW)
			{
				c->flag &= ~CACHE_FLAG_FB_OVERFLOW;
				printf("!!!fb_io: inc_head/inc_tail overflow!!!\n");
			}
			else if (c->flag & CACHE_FLAG_FIFO_OVERFLOW)
			{
				c->flag &= ~CACHE_FLAG_FIFO_OVERFLOW;
				printf("CSI_FIFO_0_OVERFLOW_IRQ!!!\n");
			}
		}
	}
}

bool cache_write_fb(struct cache_io *c, uint8_t *buf, uint32_t n)
{
	if (!buf)
	{
		c->pfb->frame_done = 1;	//end
		c->hook(c, CACHE_FLAG_FRAME);
		return true;
	}
	int tail_len = fb_io_space_tail_len(c->pfb);
	if (n > tail_len)
	{
		memcpy(fb_io_space(c->pfb), buf, tail_len);
		if (-1 == fb_io_inc_head(c->pfb, tail_len))
		{
			c->hook(c, CACHE_FLAG_FB_OVERFLOW);
		}
		memcpy(fb_io_space(c->pfb), buf + tail_len, n - tail_len);
		if (-1 == fb_io_inc_head(c->pfb, n - tail_len))
		{
			c->hook(c, CACHE_FLAG_FB_OVERFLOW);
		}
		c->hook(c, CACHE_FLAG_DMA);	// release fb_sem
	}
	else
	{
		memcpy(fb_io_space(c->pfb), buf, n);
		if (-1 == fb_io_inc_head(c->pfb, n))
		{
			c->hook(c, CACHE_FLAG_FB_OVERFLOW);
		}
		c->hook(c, CACHE_FLAG_DMA);	// release fb_sem
	}
	c->pfb->frame_size += n;
	return true;
}
int cache_start_fb(struct cache_io *c)
{
	c->pfb->frame_size = 0;
	c->pfb->frame_done = 0;
	fb_io_clear(c->pfb);
	c->t0 = OS_GetTicks();
	OS_SemaphoreRelease(&c->start_sem);
	return 0;
}

/**
 * test
 */
static struct fb_io fb;
static struct cache_io cache;
int test_cache(uint8_t ms, int total_len)
{
	uint8_t *buf;
	int len;
	int bw;

	len = 768;
	buf = (uint8_t *)malloc(len);
	memset(buf, 'X', len);

	// init
	fb.extra_size = 2048;
	fb.real_size = fb.size = 10 * 1024;
	fb.buf = (char *) malloc(fb.size + fb.extra_size);
	if (fb.buf == NULL)
	{
		printf("fb.buf malloc error\n");
		return OS_FAIL;
	}
	strcpy(cache.name, "test_cache");
	cache_config(&cache, 0x00300000, 8*64*1024);
	cache_init(&cache, cache_loop, &fb);

	// run
	if (OS_OK != cache_open(&cache))
	{
		printf("cache_open err!\n");
		return OS_FAIL;
	}
	if (cache_clear(&cache) != OS_OK)
	{
		printf("cache_clear err!\n");
		return OS_FAIL;
	}

	cache_start_fb(&cache);	// cache_io thread loop run
	for (bw = 0; bw < total_len; bw += len)
	{
		cache_write_fb(&cache, buf, len);
		OS_MSleep(ms);
	}
	cache_write_fb(&cache, NULL, 0);

	if (OS_OK != OS_SemaphoreWait(&cache.frame_sem, 2000))
	{
		printf("frame_sem: wait err!\n");
	}
	if (OS_OK != OS_SemaphoreWait(&cache.cache_sem, 2000))
	{
		printf("cache_sem: wait err!\n");
	}
	cache_close(&cache);
	printf("fb done:fb size=%d, time:%dms\n", fb.frame_size, cache.t1 - cache.t0);
	printf("cache done:cache size=%d, time:%dms\n", cache.size, cache.t2 - cache.t0);

	// exit
	cache_exit(&cache);
	if (fb.buf)
	{
		free(fb.buf);
		fb.buf = NULL;
	}
	free(buf);
	buf = NULL;

	return OS_OK;
}

/**
 * @addr 和 @len可以无需4096对齐，函数内部会处理
 */
int uni_flash_erase_v2(int addr, int len)
{
	uint32_t addr_4k_head, len_4k_head;
	uint32_t addr_64k_middle, len_64k_middle;
	uint32_t addr_4k_tail, len_4k_tail;

	if (addr % 4096)
	{
		uint32_t t1, t2;
		t1 = addr & (~4095);
		t2 = (addr + len + 4095) & (~4095);
		len = t2 - t1;
	}
	else
	{
		len = (len + 4095) & (~4095);
	}
	addr &= (~4095);
	addr_4k_head = addr;
	addr_64k_middle = (addr + 65535) & (~65535);
	if (len <= FLASH_ERASE_64KB)
	{
		len_4k_head = len;
		len_64k_middle = 0;
		addr_4k_tail = addr_4k_head;
		len_4k_tail = 0;
	}
	else
	{
		len_4k_head = addr_64k_middle - addr_4k_head;
		len_64k_middle = (len - len_4k_head) & (~65535);
		if (len_64k_middle > 0)
		{
			addr_4k_tail = addr_64k_middle + len_64k_middle;
			len_4k_tail = len - len_4k_head - len_64k_middle;
		}
		else
		{
			addr_4k_tail = addr_64k_middle;
			len_4k_tail = len - len_4k_head;
		}
	}
	printf("head:%08x,%08x  middle:%08x,%08x  tail:%08x,%08x\r\n",
		addr_4k_head, len_4k_head, addr_64k_middle, len_64k_middle, addr_4k_tail, len_4k_tail);
	
    if (HAL_OK != HAL_Flash_Open(0, 5000)) {
		return -1;
    }
	if (len_4k_head && (HAL_OK != HAL_Flash_Erase(0, FLASH_ERASE_4KB, addr_4k_head, len_4k_head/4096))) {
        HAL_Flash_Close(0);
		return -1;
    }
	if (len_64k_middle && (HAL_OK != HAL_Flash_Erase(0, FLASH_ERASE_64KB, addr_64k_middle, len_64k_middle/65536))) {
        HAL_Flash_Close(0);
		return -1;
    }
	if (len_4k_tail && (HAL_OK != HAL_Flash_Erase(0, FLASH_ERASE_4KB, addr_4k_tail, len_4k_tail/4096))) {
        HAL_Flash_Close(0);
		return -1;
    }
	
    HAL_Flash_Close(0);
    return 0;
}
