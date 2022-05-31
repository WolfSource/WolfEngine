/*********************************************************************
* Filename:   sha256.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding SHA1 implementation.
*
* File copied from the project: https://github.com/B-Con/crypto-algorithms
* License (quoting from author's github project):
* This code is released into the public domain free of any restrictions. The author requests 
* acknowledgement if the code is used, but does not require it. This code is provided free of any 
* liability and without any quality claims by the author.
*
*********************************************************************/

#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>
#include <stdint.h>

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/

typedef struct {
	uint8_t data[64];
	uint32_t datalen;
	unsigned long long bitlen;
	uint32_t state[8];
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void _librist_SHA256_Transform(SHA256_CTX *ctx, const uint8_t data[]);
void _librist_SHA256_Init(SHA256_CTX *ctx);
void _librist_SHA256_Update(SHA256_CTX *ctx, const uint8_t data[], size_t len);
void _librist_SHA256_Final(SHA256_CTX *ctx, uint8_t hash[]);

#endif   // SHA256_H
