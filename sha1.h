#ifndef SHA1_H
#define SHA1_H

#include <stdint.h>
#include <stdio.h>

typedef char byte;
typedef uint8_t u8;
typedef int32_t i32;
typedef uint32_t u32;
typedef uint64_t u64;

void SHA1Digest(void *Memory, byte *Destination, byte *Input, u32 Length);

#endif
