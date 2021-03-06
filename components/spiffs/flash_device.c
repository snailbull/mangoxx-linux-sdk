#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include "spiffs.h"

/******************************************************************************
 * flash file
 */
struct flash_file
{
	uint8_t *flash_mem;
	int size;
	int flag;		// [0]:flash init,  [1]:
	char fname[32];
	int fd;
};
static struct flash_file s_flash={
	.flash_mem = 0,
	.size = 8*1024*1024,
	.flag = 0,
	.fname = {"flash.bin"},
	.fd = -1
};

static void die (const char *what)
{
	if (errno == 0) {
		fprintf(stderr, "%s: fatal error\n", what);
	} else {
		perror(what);
	}
	exit(1);
}

/**
 * flash file: 8MByte  flash.bin
 * @*fname    "flash.bin"
 * @size   8*1024*1024
 */
int flash_init(char *fname)
{
	if (s_flash.flag)
		return 0;
	
	s_flash.fd = open(s_flash.fname, O_CREAT | O_RDWR, 0664);
	if (s_flash.fd == -1)
	{
		return -1;
	}

	s_flash.flash_mem = malloc(s_flash.size);
	//mmap (0, FS1_FLASH_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_img, 0);
	if (!s_flash.flash_mem)
	{
		return -2;
	}

	int n, br;
	lseek(s_flash.fd, 0, SEEK_SET);
	n = 0;
	while ((br = read(s_flash.fd, s_flash.flash_mem+n, 4096)))
	{
		if (br < 0)
		{
			return -3;
		}
		else if (br > 0)
		{
			n += br;
			if (n >= s_flash.size)
			{
				printf("load flash.bin success!\n");
				break;
			}
		}
	}
	s_flash.flag = 1;
	return 0;
}
int flash_flush(void)
{
	if (s_flash.flag == 0)
		return -1;
	int n, bw;
	lseek(s_flash.fd, 0, SEEK_SET);
	n = 0;
	while ((bw = write(s_flash.fd, s_flash.flash_mem+n, 4096)))
	{
		if (bw < 0)
		{
			die("write,flash_mem memory to imgfile");
		}
		else if (bw > 0)
		{
			n += bw;
			if (n >= s_flash.size)
			{
				printf("save flash.bin success!\n");
				break;
			}
		}
	}
	fsync(s_flash.fd);
	return 0;
}
int flash_exit(void)
{
	if (s_flash.flag == 0)
		return 0;
	
	int n, bw;
	lseek(s_flash.fd, 0, SEEK_SET);
	n = 0;
	while ((bw = write(s_flash.fd, s_flash.flash_mem+n, 4096)))
	{
		if (bw < 0)
		{
			die("write,flash_mem memory to imgfile");
		}
		else if (bw > 0)
		{
			n += bw;
			if (n >= s_flash.size)
			{
				printf("flash memory to imgfile success!\n");
				break;
			}
		}
	}
	free(s_flash.flash_mem);
	s_flash.flash_mem = 0;
	close(s_flash.fd);
	s_flash.flag = 0;

	return 0;
}

int flash_read(uint32_t addr, uint32_t size, uint8_t *dst)
{
	if (s_flash.flag == 0)
		return -1;
	memcpy(dst, s_flash.flash_mem + addr, size);
	return SPIFFS_OK;
}
/**
 * 模拟flash只能写0
 */
int flash_write(uint32_t addr, uint32_t size, uint8_t *src)
{
	if (s_flash.flag == 0)
		return -1;
	uint8_t *p = s_flash.flash_mem + addr;
	int i;
	for (i=0; i<size; i++) {
		p[i] &= src[i];
	}
	return SPIFFS_OK;
}
/**
 * 模拟flash擦除为0xFF, 擦除地址向下取4096对齐，擦除大小向上取4096
 */
int flash_erase(uint32_t addr, uint32_t size)
{
	if (s_flash.flag == 0)
		return -1;
	addr = addr&(~4095);
	size = (size + 4095)&(~4095);
	memset(s_flash.flash_mem + addr, 0xff, size);
	return SPIFFS_OK;
}

#ifdef ENABLE_SPIFFS_TOOLS
/******************************************************************************
 * spiffs tools
 */
#include "flash_spiffs.h"

static spiffs fs;
static uint8_t spiffs_work_buf[LOG_PAGE * 2];
static uint8_t spiffs_fd_buf[FD_BUF_SIZE];
static uint8_t spiffs_cache_buf[CACHE_BUF_SIZE];

static void list(char *path)
{
	spiffs_DIR dir;
	if (!SPIFFS_opendir (&fs, path, &dir))
	{
		die ("spiffs_opendir");
	}
	struct spiffs_dirent de;
	while (SPIFFS_readdir (&dir, &de))
	{
		static const char types[] = "?fdhs"; // file, dir, hardlink, softlink
		char name[sizeof(de.name) + 1] = { 0 };
		memcpy (name, de.name, sizeof(de.name));
		printf("%c %6u %s\n", types[de.type], de.size, name);
	}
	SPIFFS_closedir (&dir);
}

static void cat(char *fname)
{
	spiffs_file fh = SPIFFS_open (&fs, fname, SPIFFS_RDONLY, 0);
	char buf[512];
	s32_t n;
	while ((n = SPIFFS_read (&fs, fh, buf, sizeof (buf))) > 0)
	{
		write (STDOUT_FILENO, buf, n);
	}
	SPIFFS_close (&fs, fh);
}

static void import(char *src, char *dst)
{
	int fd = open(src, O_RDONLY);
	if (fd < 0)
	{
		die (src);
	}

	spiffs_file fh = SPIFFS_open (&fs, dst, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_WRONLY, 0);
	if (fh < 0)
	{
		die ("spiffs_open");
	}

	char buf[512];
	s32_t n;
	while ((n = read (fd, buf, sizeof (buf))) > 0) {
		if (SPIFFS_write (&fs, fh, buf, n) < 0)
		{
			die ("spiffs_write");
		}
	}

	SPIFFS_close (&fs, fh);
	close (fd);
}

static void export(char *src, char *dst)
{
	spiffs_file fh = SPIFFS_open (&fs, src, SPIFFS_RDONLY, 0);
	if (fh < 0)
	{
		die ("spiffs_open");
	}

	int fd = open (dst, O_CREAT | O_TRUNC | O_WRONLY, 0664);
	if (fd < 0)
	{
		die (dst);
	}

	char buf[512];
	s32_t n;
	while ((n = SPIFFS_read (&fs, fh, buf, sizeof (buf))) > 0)
		if (write (fd, buf, n) < 0)
		{
			die ("write");
		}

	SPIFFS_close (&fs, fh);
	close (fd);
}

char *trim (char *in)
{
	if (!in)
	{
		return "";
	}

	char *out = 0;
	while (*in)
	{
		if (!out && !isspace (*in))
		{
			out = in;
		}
		++in;
	}
	if (!out)
	{
		return "";
	}
	while (--in > out && isspace (*in))
		;
	in[1] = 0;
	return out;
}

static void syntax(void)
{
	fprintf (stderr,
		"Syntax: mkspiffs -f <filename> -c [-l | -i | -r <scriptname> ]\n\n");
	exit (1);
}
/**
 * mkspiffs -f flash.bin -c -r fs.lst
 */
int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		syntax();
	}

	int opt;
	int retcode = 0;
	const char *fname = 0;
	bool create = false;
	enum { CMD_NONE, CMD_LIST, CMD_INTERACTIVE, CMD_SCRIPT } command = CMD_NONE;
	const char *script_name = 0;
	int flash_idx=0;
	int fd;
	int br,bw;

	while ((opt = getopt (argc, argv, "f:lir:c")) != -1)
	{
		switch (opt)
		{
		case 'f':
			fname = optarg;
			break;
		case 'c':
			create = true;
			break;
		case 'l':
			command = CMD_LIST;
			break;
		case 'i':
			command = CMD_INTERACTIVE;
			break;
		case 'r':
			command = CMD_SCRIPT;
			script_name = optarg;
			break;
		default:
			die ("unknown option");
		}
	}

	if (!fname)
	{
		die("Need a filename");
	}
	strcpy(s_flash.fname, fname);
	flash_init();
	if (create)
	{
		flash_erase(0, s_flash.size);
	}

	// op flash_mem memory by SPIFFS_mount
	spiffs_config cfg;
	cfg.phys_size = FS1_FLASH_SIZE;
	cfg.phys_addr = FS1_FLASH_ADDR;
	cfg.phys_erase_block = SECTOR_SIZE;
	cfg.log_block_size = LOG_BLOCK;
	cfg.log_page_size = LOG_PAGE;
	cfg.hal_read_f = flash_read;
	cfg.hal_write_f = flash_write;
	cfg.hal_erase_f = flash_erase;
	if (cfg.phys_size < 4 * cfg.log_block_size) {
		die("disk not large enough for four blocks");
	}

	if (SPIFFS_mount (&fs, &cfg,
	                  spiffs_work_buf,
	                  spiffs_fd_buf, sizeof(spiffs_fd_buf),
	                  spiffs_cache_buf, CACHE_BUF_SIZE, 0) != 0) {
		if (create) {
			if (SPIFFS_format(&fs) != 0) {
				die("spiffs_format");
			}
			if (SPIFFS_mount (&fs, &cfg,
			                  spiffs_work_buf,
			                  spiffs_fd_buf, sizeof(spiffs_fd_buf),
			                  spiffs_cache_buf, CACHE_BUF_SIZE, 0) != 0) {
				die ("spiffs_mount");
			}
			if (command == CMD_INTERACTIVE) {
				printf("Created filesystem -- size 0x%x, block_size=%d\n", cfg.phys_size, cfg.log_block_size);
			}
		} else {
			die ("spiffs_mount");
		}
	}

	// operate flash_mem memory
	if (command == CMD_NONE)
		; // maybe just wanted to create an empty image?
	else if (command == CMD_LIST)
	{
		list("/");
	}
	else
	{
		// interactive, spiffs.lst
		FILE *in = (command == CMD_INTERACTIVE) ? stdin : fopen (script_name, "r");
		if (!in)
		{
			die ("fopen");
		}
		char buf[128] = { 0 };
		if (in == stdin)
		{
			printf("> ");
		}
		while (fgets(buf, sizeof (buf) - 1, in))
		{
			char *line = trim (buf);
			if (!line[0] || line[0] == '#')
			{
				continue;
			}
			if (strcmp (line, "ls") == 0)
			{
				list("/");
			}
			else if (strncmp (line, "import ", 7) == 0)
			{
				char *src = 0, *dst = 0;
				if (sscanf (line + 7, " %ms %ms", &src, &dst) != 2)
				{
					fprintf (stderr, "SYNTAX ERROR: %s\n", line);
					retcode = 1;
				}
				else
				{
					import (src, dst);
				}
				free (src);
				free (dst);
			}
			else if (strncmp (line, "export ", 7) == 0)
			{
				char *src = 0, *dst = 0;
				if (sscanf (line + 7, " %ms %ms", &src, &dst) != 2)
				{
					fprintf (stderr, "SYNTAX ERROR: %s\n", line);
					retcode = 1;
				}
				else
				{
					export (src, dst);
				}
				free (src);
				free (dst);
			}
			else if (strncmp (line, "rm ", 3) == 0)
			{
				if (SPIFFS_remove (&fs, trim (line + 3)) < 0)
				{
					fprintf (stderr, "FAILED: %s\n", line);
					retcode = 1;
				}
			}
			else if (strncmp (line, "cat ", 4) == 0)
			{
				cat (trim (line + 4));
			}
			else if (strncmp (line, "info", 4) == 0)
			{
				u32_t total, used;
				if (SPIFFS_info (&fs, &total, &used) < 0)
				{
					fprintf (stderr, "FAILED: %s\n", line);
					retcode = 1;
				}
				else
				{
					printf ("Total: %u, Used: %u\n", total, used);
				}
			}
			else
			{
				printf ("SYNTAX ERROR: %s\n", line);
				retcode = 1;
			}

			if (in == stdin)
			{
				printf ("> ");
			}
		}
		if (in == stdin)
		{
			printf ("\n");
		}
	}

	SPIFFS_unmount(&fs);
	flash_exit();
	return retcode;
}
#endif
