#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "img_converters.h"
#include "bitmap.h"

#if 1
#define CMD_LOG(fmt, ...)		printf("[%s,%d] " fmt "\r\n", __func__,__LINE__, ##__VA_ARGS__)
#else
#define CMD_LOG(fmt, ...)
#endif

camera_fb_t s_camera;
static char file_name[128];

static int xchg_line(uint8_t *data, int w, int h, int b)
{
	uint8_t *buf;
	int i;

	buf = (uint8_t*)malloc(w*b+64);
	if (buf == NULL)
	{
		CMD_LOG("mem err!");
		return -1;
	}

	for (i = 0; i < h/2; i++)
	{
		memcpy(buf, data+i*w*b, w*b);
		memcpy(data+i*w*b, data+(h-i-1)*w*b, w*b);
		memcpy(data+(h-i-1)*w*b, buf, w*b);
	}
	free(buf);
	buf = 0;
	return 0;
}

static char *file_type(char *file)
{
	char *p = strrchr(file, '.');
	if (p == NULL)
		return NULL;
	return p;
}

int camera_init(void)
{
	int fd;
	s_camera.out = 0;
	s_camera.out_len = 0;
	s_camera.len = 4*1024*1024;
	s_camera.buf = (uint8_t*)malloc(s_camera.len);
	if (s_camera.buf == NULL)
	{
		CMD_LOG("mem err!");
		return -1;
	}
	CMD_LOG("s_camera.buf:%08x  s_camera.len:%d", s_camera.buf, s_camera.len);
	// md(s_camera.buf, 128, 1);

	return 0;
}
int camera_exit(void)
{
	if (s_camera.buf)
	{
		free(s_camera.buf);
		s_camera.buf = 0;
	}
	return 0;
}

int camera_run(char *file)
{
	FILE *fp;
	int br, bw;
	int n,i;
	int ret=0;
	uint8_t tmp[128], tmplen=128;
	int data_off, line_off, bytes_per_pix;
	
	// load fb
	strcpy(file_name, file);
	fp = fopen(file, "rb");
	if (fp == NULL)
	{
		CMD_LOG("open err!");
		return -1;
	}

	if (strcmp(".bmp", file_type(file)) == 0)
	{
		br = fread(tmp, 1,tmplen, fp);
		CMD_LOG("read %d byte.", br);
		if (*(uint16_t*)(tmp+28) == 16)
		{
			s_camera.width = *(long*)(tmp+18);
			s_camera.height = *(long*)(tmp+22);
			s_camera.format = PIXFORMAT_RGB565;
			bytes_per_pix = 2;
		}
		else if (*(uint16_t*)(tmp+28) == 24)
		{
			s_camera.width = *(long*)(tmp+18);
			s_camera.height = *(long*)(tmp+22);
			s_camera.format = PIXFORMAT_RGB888;
			bytes_per_pix = 3;
		}
		data_off = *(uint32_t*)(tmp+10);
		fseek(fp, data_off, SEEK_SET);
	}
	else if (strcmp(".yuv", file_type(file)) == 0)
	{
		s_camera.width = 1920;
		s_camera.height = 1080;
		s_camera.format = PIXFORMAT_YUV422;
		bytes_per_pix = 2;
	}

	printf("s_camera.len:%d\n", s_camera.len);
	printf("s_camera.width:%d\n", s_camera.width);
	printf("s_camera.height:%d\n", s_camera.height);
	printf("s_camera.format:%d\n", s_camera.format);
	printf("fpos:%d\n", ftell(fp));

	n = 0;
	for (;;)
	{
		br = fread(s_camera.buf+n, 1,4096, fp);
		if (br < 0)
		{
			CMD_LOG("read err!");
			ret = -1;
			goto EXIT;
		}
		if (br > 0)
		{
			n += br;
		}
		else
		{
			CMD_LOG("get fb ok! %d.", br);
			break;
		}
	}

	if (strcmp(".bmp", file_type(file)) == 0)
	{
		xchg_line(s_camera.buf, s_camera.width, s_camera.height, bytes_per_pix);
	}

EXIT:
	fclose(fp);
	return ret;
}

int camera_compress(void)
{
	int bw, n;
	FILE *fp;
	int ret = 0;

	// compress to jpg
	if (frame2jpg(&s_camera, 85, &s_camera.out, &s_camera.out_len))
	{
		CMD_LOG("frame2jpg ok!");
	}
	else
	{
		CMD_LOG("frame2jpg fail!");
		return 0;
	}
	CMD_LOG("out:%p  out_len:%d", s_camera.out, s_camera.out_len);

	// save buf to jpg file
	strcpy(file_type(file_name), ".jpg");
	fp = fopen(file_name, "w+");
	if (fp == NULL)
	{
		CMD_LOG("open err!");
		return -1;
	}

	n = 0;
	for (;;)
	{
		bw = fwrite(s_camera.out+n, 1,s_camera.out_len-n, fp);
		if (bw < 0)
		{
			CMD_LOG("write err!");
			ret = -1;
			break;
		}
		if (bw > 0)
		{
			n += bw;
			CMD_LOG("write %d byte.", bw);
		}
		else
		{
			CMD_LOG("write jpg ok!");
			break;
		}
	}

	fclose(fp);

	free(s_camera.out);
	s_camera.out = 0;

	return ret;
}

struct bitmap_info{
	uint8_t a;
	uint8_t b;
	uint16_t c;
	char d[8];
};

int test_bmp(void)
{
	CMD_LOG("BITMAPINFO:%d  BITMAPFILEHEADER:%d BITMAPINFOHEADER:%d\n", sizeof(BITMAPINFO), sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
	printf("BITMAPINFO:%d  BITMAPFILEHEADER:%d BITMAPINFOHEADER:%d\n", sizeof(BITMAPINFO), sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
	printf("struct bitmap_info:%d\n", sizeof(struct bitmap_info));
	printf("camera_fb_t:%d\n", sizeof(camera_fb_t));
	return 0;
}
