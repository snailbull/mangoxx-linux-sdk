#ifndef _FCMD_CFG_H_
#define _FCMD_CFG_H_
/*******************************************************************************
* 用户函数命令头文件包含，函数声明
*/

#include "cmd_mem.h"
#include "cJSON_test/cjson_test.h"
#include "mango_httpc_test/mango_httpc_test.h"
#include "cmd_jpge.h"
#include "spiffs/flash_device.h"
#include "spiffs/flash_spiffs.h"

/**
 * 系统命令表
 */
CmdTbl_t CmdSysTbl[] =
{
	{"l", (void(*)(void))sys_ls},
	{"h", (void(*)(void))sys_h},
	{"q", (void(*)(void))sys_q},
};
/**
 * 函数命令表
 */
CmdTbl_t CmdTbl[] =
{
	{"void md(int addr, int elem_cnt, int elem_size)", (void(*)(void))md},
	{"int cmp(void *addr1, void *addr2, int elem_cnt, int elem_size)", (void(*)(void))cmp},
	{"int sfmd(uint32_t addr, int elem_cnt, int elem_size)", (void(*)(void))sfmd},
	{"int sfmw(uint32_t writeaddr, uint8_t *pbuf, uint32_t num)", (void(*)(void))sfmw},
	{"int sfmc(uint32_t erase_addr, uint32_t erase_mode, uint32_t num)", (void(*)(void))sfmc},
	{"int sfmu(uint32_t start_addr, uint32_t sectors)", (void(*)(void))sfmu},
	{"int flash_init(char *fname)", (void(*)(void))flash_init},
	{"int flash_exit(void)", (void(*)(void))flash_exit},
	{"int flash_flush(void)", (void(*)(void))flash_flush},
	{"int platform_spiffs_init(void)", (void(*)(void))platform_spiffs_init},
	{"int platform_spiffs_exit(int format)", (void(*)(void))platform_spiffs_exit},
	{"int test_list(char *path)", (void(*)(void))test_list},
	{"int test_cat(char *fname)", (void(*)(void))test_cat},
	{"int test_import(char *fname, char *src, int srclen)", (void(*)(void))test_import},

	{"void memset(void *s, int c, size_t n)", (void(*)(void))memset},
	{"void *memcpy(void *s1, const void *s2, size_t n)", (void(*)(void))memcpy},
	{"void *malloc(size_t size)", (void(*)(void))malloc},
	{"void free(void *ptr)", (void(*)(void))free},
	{"long strtol(const char *nptr, char **endptr, int base)", (void(*)(void))strtol},
	{"char *strstr(const char *s1, const char *s2)", (void(*)(void))strstr},
	{"FILE *fopen(const char *file,const char *mode)", (void(*)(void))fopen},
	{"int fclose(File *fp)", (void(*)(void))fclose},
	{"size_t fwrite(const void *p,size_t size,size_t nmemb, FILE *fp)", (void(*)(void))fwrite},
	{"size_t fread(void *p, size_t size, size_t nmenb, FILE *fp)", (void(*)(void))fread},
	{"int strncmp(char *s1,char *s2,int n)",	(void(*)(void))strncmp},
	{"int printf(char *fmt,...)",	(void(*)(void))printf},

	{"int cJSON_test(void)", (void(*)(void))cJSON_test},
	{"void json_test_init(int select)", (void(*)(void))json_test_init},
	{"void json_test_exit(void)", (void(*)(void))json_test_exit},
	{"void json_struct_print(void *json)", (void(*)(void))json_struct_print},
	{"void create_objects()", (void(*)(void))create_objects},
	{"void tuya_json_test(void)", (void(*)(void))tuya_json_test},
	{"void tuya_json_test2(char *dps_json)", (void(*)(void))tuya_json_test2},
	{"void json_array_test(int i)", (void(*)(void))json_array_test},
	{"void json_test_print(void)", (void(*)(void))json_test_print},
	{"void speech_json_config(void)", (void(*)(void))speech_json_config},

	{"int basicAuth_test(void)", (void(*)(void))basicAuth_test},
	{"int get_test(char *server, uint16_t port, char *url)", (void(*)(void))get_test},
	{"int sslget_test(char *server, uint16_t port, char *url)", (void(*)(void))sslget_test},
	{"int persistent_test(void)", (void(*)(void))persistent_test},
	{"int post_test(void)", (void(*)(void))post_test},
	{"int shoutcast_test(void)", (void(*)(void))shoutcast_test},
	{"int websockets_test(void)", (void(*)(void))websockets_test},
	{"int ssl_client(void)", (void(*)(void))ssl_client},
	{"int ssl_server(void)", (void(*)(void))ssl_server},

	{"int camera_init(void)", (void(*)(void))camera_init},
	{"int camera_exit(void)", (void(*)(void))camera_exit},
	{"int camera_run(char *file)", (void(*)(void))camera_run},
	{"int camera_compress(void)", (void(*)(void))camera_compress}

};
uint8_t CmdSysTblSize = sizeof(CmdSysTbl) / sizeof(CmdSysTbl[0]);
uint8_t CmdTblSize = sizeof(CmdTbl) / sizeof(CmdTbl[0]);

#endif
