// SPDX-FileCopyrightText: 2024 Integral <integral@member.fsf.org>
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef IOCTL_H
#define IOCTL_H

#include <stdint.h>

#define AES256_KEYLEN 32

typedef struct _aes256_key_t {
	uint8_t key[AES256_KEYLEN];
} aes256_key_t;

#define SET_KEY _IOW('e', 'k', aes256_key_t *)
#define SET_ENCRYPT _IO('e', 'e')
#define SET_DECRYPT _IO('e', 'd')
#define START_WRITE _IO('e', 'w')
#define START_READ _IO('e', 'r')
#define RESET _IO('e', 0)

#endif
