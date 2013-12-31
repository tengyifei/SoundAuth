#ifndef __SHA1_H
#define __SHA1_H

#define uint32_t unsigned long
#define uint8_t unsigned char
#define size_t unsigned long
#define INT32U unsigned long

typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t  buffer[64];
} SHA1_CTX;

#define SHA1_DIGEST_SIZE 20

void SHA1_Init(SHA1_CTX* context);
void SHA1_Update(SHA1_CTX* context, const uint8_t* data, const size_t len);
void SHA1_Final(SHA1_CTX* context, uint8_t digest[SHA1_DIGEST_SIZE]);
unsigned char* Sha1(void* ptr, int n);

#endif /* __SHA1_H */
