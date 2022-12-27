#ifndef TYPE_H
#define TYPE_H

/* Instead of using 'chars' to allocate non-character bytes,
 * we will use these new type with no semantic meaning */
typedef unsigned int   u32;
typedef          int   s32;
typedef unsigned short u16;
typedef          short s16;
typedef unsigned char  u8;
typedef          char  s8;

typedef unsigned int   uint32;
typedef          int   int32;
typedef unsigned short uint16;
typedef          short int16;
typedef unsigned char  uint8;
typedef          char  int8;

typedef int bool;
#define true 1
#define false 0

#define low_16(address) (u16)((address) & 0xFFFF)
#define high_16(address) (u16)(((address) >> 16) & 0xFFFF)

#endif
