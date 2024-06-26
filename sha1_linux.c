#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "sha1.h"

#define MEMORY_SIZE 1024 * 1024 * 1024

int main(int argc, char **argv)
{
    u8 *Memory = (u8 *)mmap(0, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    u32 MemoryOffset = 0;
    byte Digest[41];

    // TODO: We're not really handling > 4gb because of u32s

    if (argc == 1)
    {
        u8 Character = 0;
        while (read(0, &Character, 1) > 0)
        {
            Memory[MemoryOffset++] = Character;
        }

	// NOTE: A offset of 0 will be considered an empty string
        u32 Length = MemoryOffset;
        u8 *HashMemory = Memory + MemoryOffset + 1;
        byte *Input = (byte *)Memory;

        SHA1Digest(HashMemory, Digest, Input, Length);
        printf("%s  -\n", Digest);
    }
    else
    {
        i32 FileDescriptor = 0;
        i32 Length = 0;

        while (*++argv)
        {
            FileDescriptor = open(*argv, O_RDONLY);

            if (FileDescriptor < 0)
            {
                printf("sha1: %s: No such file\n", *argv);
		return 1;
            }
            else
            {
                Length = (i32)lseek(FileDescriptor, 0, SEEK_END);

                if (Length > 0)
                {
                    byte *FileContents = (byte *)mmap(0, (u32)Length, PROT_READ, MAP_PRIVATE, FileDescriptor, 0);
                    SHA1Digest(Memory + MemoryOffset, Digest, FileContents, (u32)Length);
                    printf("%s  %s\n", Digest, *argv);
                }
                else
                {
                    printf("sha1: %s: Empty file\n", *argv);
		    return 1;
                }
            }
        }
    }
    
    return 0;
}
