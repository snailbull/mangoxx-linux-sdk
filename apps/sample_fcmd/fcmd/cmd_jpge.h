#ifndef _CMD_JPGE_H_
#define _CMD_JPGE_H_

int camera_init(void);
int camera_exit(void);
int camera_run(char *file);
int camera_compress(void);

int test_bmp(void);

#endif
