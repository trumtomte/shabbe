#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: make more test and see what improvements can be made

uint32_t rotl(const uint32_t value, const unsigned int count) {
  return value << count | value >> (32 - count);
}

void ui32be_copy(uint32_t *dst, const uint8_t *src) {
  union {
    uint32_t ui32be;
    uint8_t bytes[4];
  } u;

  u.bytes[0] = src[3];
  u.bytes[1] = src[2];
  u.bytes[2] = src[1];
  u.bytes[3] = src[0];

  *dst = u.ui32be;

  // NOTE: alternative solution, not sure if it's more performant
  // #include <arpa/inet.h>
  // uint32_t srcbe;
  // memcpy(&srcbe, src, 4);
  // *dst = htonl(srcbe);
}

void ui64be_copy(uint8_t *dst, const uint64_t src) {
  union {
    uint64_t ui64;
    uint8_t bytes[8];
  } u;

  u.ui64 = src;

  dst[0] = u.bytes[7];
  dst[1] = u.bytes[6];
  dst[2] = u.bytes[5];
  dst[3] = u.bytes[4];
  dst[4] = u.bytes[3];
  dst[5] = u.bytes[2];
  dst[6] = u.bytes[1];
  dst[7] = u.bytes[0];

  // NOTE: perhaps a more performant solution
  // dst[0] = (uint8_t)(src >> 56 & 0xff);
  // dst[1] = (uint8_t)(src >> 48 & 0xff);
  // dst[2] = (uint8_t)(src >> 40 & 0xff);
  // dst[3] = (uint8_t)(src >> 32 & 0xff);
  // dst[4] = (uint8_t)(src >> 24 & 0xff);
  // dst[5] = (uint8_t)(src >> 16 & 0xff);
  // dst[6] = (uint8_t)(src >> 8 & 0xff);
  // dst[7] = (uint8_t)(src >> 0 & 0xff);
}

int sha1digest(char *dst, const char *input, const size_t isize) {
  size_t msize = isize < 64 ? 64 : isize;

  while (msize % 64)
    msize += 64 - (msize % 64);

  uint8_t *msg = malloc(msize * sizeof(uint8_t));
  // Set all memory to zeros
  memset(msg, 0, msize);
  // Copy the input message
  memcpy(msg, input, isize);
  // Set the trailing '1' bit
  msg[isize] = 0x80;
  // Append the msg_size (as uint64 BE) to the end
  ui64be_copy(&msg[msize - 8], isize * 8);

  uint32_t h0 = 0x67452301;
  uint32_t h1 = 0xEFCDAB89;
  uint32_t h2 = 0x98BADCFE;
  uint32_t h3 = 0x10325476;
  uint32_t h4 = 0xC3D2E1F0;
  uint32_t a, b, c, d, e, f, k, temp;

  // Break into 64 byte (512 bit) chunks
  for (size_t chunk = 0; chunk < msize / 64; chunk++) {
    uint32_t ws[80];

    a = h0;
    b = h1;
    c = h2;
    d = h3;
    e = h4;

    for (int i = 0; i < 80; i++) {
      // 0-15 are the 32-bit words
      if (i < 16)
        ui32be_copy(&ws[i], &msg[(chunk * 64) + (i * 4)]);
      // extended into the 80 32-bit words
      if (i >= 16)
        ws[i] = rotl((ws[i - 3] ^ ws[i - 8] ^ ws[i - 14] ^ ws[i - 16]), 1);

      if (i < 20) {
        f = (b & c) | ((~b) & d);
        k = 0x5A827999;
      } else if (i < 40) {
        f = b ^ c ^ d;
        k = 0x6ED9EBA1;
      } else if (i < 60) {
        f = (b & c) | (b & d) | (c & d);
        k = 0x8F1BBCDC;
      } else if (i < 80) {
        f = b ^ c ^ d;
        k = 0xCA62C1D6;
      }

      temp = rotl(a, 5) + f + e + k + ws[i];
      e = d;
      d = c;
      c = rotl(b, 30);
      b = a;
      a = temp;
    }

    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
  }

  free(msg);
  snprintf(dst, 41, "%08x%08x%08x%08x%08x", h0, h1, h2, h3, h4);
  return 0;
}

void run_test(char *test, char *expected) {
  char digest[41];
  sha1digest(digest, test, strlen(test));
  printf("TESTING:  %s\n", test);
  printf("RECEIVED: %s\n", digest);
  printf("EXPECTED: %s\n\n", expected);
}

void run_tests(void) {
  run_test("The quick brown fox jumps over the lazy dog",
           "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");
  run_test("The quick brown fox jumps over the lazy dogaaaaaaaaaaaaaaaaaaaaaa",
           "10a883c4b50f9c1d9775aad5d5605788dc2be79e");
  run_test("", "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s \"string to hash\"\n", argv[0]);
    printf("   or: %s -t to print test results\n", argv[0]);
    return 0;
  }

  if (strcmp(argv[1], "-t") == 0) {
    run_tests();
    return 0;
  }

  char digest[41];
  sha1digest(digest, argv[1], strlen(argv[1]));
  printf("%s\n", digest);

  return 0;
}
