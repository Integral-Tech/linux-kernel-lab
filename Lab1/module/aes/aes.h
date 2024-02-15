// SPDX-FileCopyrightText: 2024 Integral <integral@member.fsf.org>
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef AES_H
#define AES_H

#include <linux/types.h>

#define AES_BLOCKLEN 16 // Block length in bytes - AES is 128b block only
#define COLUMNS 4
#define AES256

#if defined(AES256)
#define AES_KEYLEN 32
#define AES_KEYEXPSIZE 240
#define WORDS 8
#define ROUNDS 14
#elif defined(AES192)
#define AES_KEYLEN 24
#define AES_KEYEXPSIZE 208
#define WORDS 6
#define ROUNDS 12
#else
#define AES_KEYLEN 16 // Key length in bytes
#define AES_KEYEXPSIZE 176
#define WORDS 4 // The number of 32 bit words in a key.
#define ROUNDS 10 // The number of rounds in AES cipher.
#endif

#define get_sbox_val(num) (sbox[(num)])
#define get_sbox_inv(num) (inv_sbox[(num)])

#define xtime(x)                                        \
	({                                              \
		typeof(x) _x = (x);                     \
		((_x << 1) ^ (((_x >> 7) & 1) * 0x1b)); \
	})

typedef uint8_t state_t[4][4];

typedef struct _aes_ctx_t {
	uint8_t round_key[AES_KEYEXPSIZE];
	uint8_t iv[AES_BLOCKLEN]; // Initial Vector
} aes_ctx_t;

void aes_init_ctx_iv(aes_ctx_t *ctx, const uint8_t *key, const uint8_t *iv);
void aes_ctx_set_iv(aes_ctx_t *ctx, const uint8_t *iv);

void aes_cbc_encrypt_buffer(aes_ctx_t *ctx, uint8_t *buf, size_t length);
void aes_cbc_decrypt_buffer(aes_ctx_t *ctx, uint8_t *buf, size_t length);

#endif
