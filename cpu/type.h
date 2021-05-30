#ifndef type_H
#define type_H

/* Instead of using 'chars' to allocate non-character bytes,
 * we will use these new type with no semantic meaning */
//typedef unsigned int u32;
//typedef int s32;
//typedef unsigned short uint16_t;
//typedef short s16;
//typedef unsigned char u8;
//typedef char s8;

#define low_16(address) (uint16_t)((address)&0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

#endif