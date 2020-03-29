#ifndef _SPIFFS_TYPEDEFS_H_
#define _SPIFFS_TYPEDEFS_H_

#include "c_types.h"
#include "osapi.h"

typedef  int32_t s32_t;
typedef uint32_t u32_t;
typedef  int16_t s16_t;
typedef uint16_t u16_t;
typedef   int8_t s8_t;
typedef  uint8_t u8_t;

typedef signed int ptrdiff_t;

#if !defined(offsetof)
#define offsetof(s, m)   (size_t)&(((s *)0)->m)
#endif 

#if !defined(__size_t)
  #define __size_t 1
  typedef unsigned int size_t;   /* others (e.g. <stdio.h>) also define */
   /* the unsigned integral type of the result of the sizeof operator. */
#endif

#undef NULL  /* others (e.g. <stdio.h>) also define */
#define NULL 0
   /* null pointer constant. */

#endif
