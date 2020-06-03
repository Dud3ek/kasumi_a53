#pragma once

#ifndef __KASUMI_H__
#define __KASUMI_H__

// definitions of all variables we will be using

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef union {
	u32 b32[2];
	u16 b16[4];
	u8	b8[8];
} Struct1;


typedef union {
	u32 k32[2];
	u16 k16[4];
} testing;


void KeySchedule(u8* key);
void Kasumi(u8* data);
void KasumiDecrypt(u8* data);

#endif