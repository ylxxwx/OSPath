#ifndef _TYPE_H_
#define _TYPE_H_

#include <stdint.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char s8;
typedef short s16;
typedef int s32;

// little endian is default for intel x86
typedef uint32_t __le32;
typedef uint16_t __le16;
typedef uint8_t  __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;


#endif // _TYPE_H_
