#ifndef _FB_IO_H_
#define _FB_IO_H_

// DMA fb ringbuf
struct fb_io
{
	char	*buf;		// IO Buffer
	int		size;		// 正常大小
	volatile int 	real_size;	// 可能会动态变更大小!!! (real_size == size) or (size < real_size < size+extra_size)
	// 数据被读后，恢复为(real_size == size)
	int 			extra_size;	// 为保证单次DMA连续，额外增加2k内存，适应ringbuf尾部边界的DMA单次传输！

	volatile int	head;		// write addr
	volatile int	tail;		// read addr
	volatile int	len;		// bytes can read

	char	frame_done;	// 一帧结束标志
	int		frame_size;	// 一次拍照读到的总字节数
};

/**
 * fb io ring buffer
 */
__inline char *fb_io_buf(struct fb_io *io)
{
	return (io->buf);
}
__inline void fb_io_clear(struct fb_io *io)
{
	io->len = io->tail = io->head = 0;
	io->real_size = io->size;
}

__inline char *fb_io_space(struct fb_io *io)
{
	return (io->buf + io->head);
}

__inline char *fb_io_data(struct fb_io *io)
{
	return (io->buf + io->tail);
}

__inline int fb_io_space_len(const struct fb_io *io)
{
	return (io->real_size - io->len);
}

__inline int fb_io_space_tail_len(const struct fb_io *io)
{
	if (io->head > io->tail)
	{
		return (io->real_size - io->head);
	}
	else
	{
		return (io->real_size - io->len);
	}
}

// 如果(head <= tail)，返回tail到尾部边界的数据长度，保证返回的数据内存连续！
__inline int fb_io_data_len(const struct fb_io *io)
{
	if ((io->head <= io->tail) && (io->len > 0))
	{
		return (io->real_size - io->tail);
	}
	else
	{
		return (io->len);
	}
}

__inline int fb_io_data_total_len(const struct fb_io *io)
{
	return (io->len);
}

__inline int fb_io_inc_tail(struct fb_io *io, int n)
{
	if ((io->len - n) < 0)
	{
		return -1;    // ringbuf下溢！禁止读出！
	}
	io->tail += n;
	if (io->tail >= io->real_size)
	{
		// inc_tail读到尾部，恢复real_size大小
		io->tail = 0;
		io->real_size = io->size;
	}
	io->len -= n;
	return n;
}

__inline int fb_io_inc_head(struct fb_io *io, int n)
{
	if ((io->len + n) > io->real_size)
	{
		return -1;    // ringbuf上溢! 禁止写入!
	}
	io->head += n;
	if (io->head >= io->real_size)
	{
		io->head = 0;
	}
	io->len += n;
	return n;
}
// 判断inc_head是否越界，设置real_size，适应ringbuf尾部边界的DMA单次传输
__inline void fb_io_resize(struct fb_io *io, int n)
{
	if (((io->head + n) > io->size) &&
	        ((io->head + n) < (io->size + io->extra_size)))
	{
		io->real_size = io->head + n;
	}
}

#endif
