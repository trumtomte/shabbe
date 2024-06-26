#include "sha1.h"

void SHA1Digest(void *Memory, byte *Destination, byte *Input, u32 Length)
{
    u32 Size = Length < 64 ? 64 : Length;
    // Size must be a multiple of 64
    while (Size % 64)
    {
        Size += 64 - (Size % 64);
    }

    // NOTE: We expect this to be enough memory (i.e. Size * u8)
    u8 *Message = (u8 *)Memory;

    // Copy the input and set remaining memory to zero
    for (u32 Index = 0; Index < Size; Index++)
    {
        Message[Index] = Index < Length ? (u8)Input[Index] : 0;
    }

    // Trailing '1' bit
    Message[Length] = 0x80;

    // Trailing input length in bits as a u64 in Big Endian
    u64 BitLength = (u64)(Length * 8);
    Message[Size - 8] = (u8)(BitLength >> 56 & 0xff);
    Message[Size - 7] = (u8)(BitLength >> 48 & 0xff);
    Message[Size - 6] = (u8)(BitLength >> 40 & 0xff);
    Message[Size - 5] = (u8)(BitLength >> 32 & 0xff);
    Message[Size - 4] = (u8)(BitLength >> 24 & 0xff);
    Message[Size - 3] = (u8)(BitLength >> 16 & 0xff);
    Message[Size - 2] = (u8)(BitLength >> 8 & 0xff);
    Message[Size - 1] = (u8)BitLength;

    u32 H0 = 0x67452301;
    u32 H1 = 0xEFCDAB89;
    u32 H2 = 0x98BADCFE;
    u32 H3 = 0x10325476;
    u32 H4 = 0xC3D2E1F0;
    u32 A, B, C, D, E, F, K, Temp;

    for (u32 Chunk = 0; Chunk < (Size / 64); Chunk++)
    {
        u32 Words[80];
        A = H0;
        B = H1;
        C = H2;
        D = H3;
        E = H4;

        for (u32 WordIndex = 0; WordIndex < 80; WordIndex++)
        {
            if (WordIndex < 16)
            {
                Temp = (Chunk * 64) + (WordIndex * 4);
                // Make this word a u32 Big Endian
                Words[WordIndex] = ((u32)Message[Temp] << 24) + ((u32)Message[Temp + 1] << 16) +
                                   ((u32)Message[Temp + 2] << 8) + (u32)Message[Temp + 3];
            }

            if (WordIndex >= 16)
            {
                Temp = Words[WordIndex - 3] ^ Words[WordIndex - 8] ^ Words[WordIndex - 14] ^ Words[WordIndex - 16];
                Words[WordIndex] = Temp << 1 | Temp >> 31;
            }

            if (WordIndex < 20)
            {
                F = (B & C) | ((~B) & D);
                K = 0x5A827999;
            }
            else if (WordIndex < 40)
            {
                F = B ^ C ^ D;
                K = 0x6ED9EBA1;
            }
            else if (WordIndex < 60)
            {
                F = (B & C) | (B & D) | (C & D);
                K = 0x8F1BBCDC;
            }
            else if (WordIndex < 80)
            {
                F = B ^ C ^ D;
                K = 0xCA62C1D6;
            }

            Temp = (A << 5 | A >> 27) + F + E + K + Words[WordIndex];
            E = D;
            D = C;
            C = B << 30 | B >> 2;
            B = A;
            A = Temp;
        }

        H0 += A;
        H1 += B;
        H2 += C;
        H3 += D;
        H4 += E;
    }

    snprintf(Destination, 41, "%08x%08x%08x%08x%08x", H0, H1, H2, H3, H4);
}
