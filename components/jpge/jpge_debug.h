#ifndef _JPGE_DEBUG_H_
#define _JPGE_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#if 1
#define ESP_LOG(fmt, ...)		printf("[%s,%d] " fmt "\r\n", __func__,__LINE__, fmt, ##__VA_ARGS__)
#else
#define ESP_LOG(fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif
