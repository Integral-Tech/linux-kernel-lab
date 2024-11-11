// SPDX-FileCopyrightText: 2024 Integral <integral@member.fsf.org>
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef IOCTL_H
#define IOCTL_H

#include "aes/aes.h"

#define CDEV_NAME "encrypt"

typedef struct _aes_key_t {
	uint8_t key[AES_KEYLEN];
} aes_key_t;

enum { DECRYPT, ENCRYPT };

#define SET_KEY _IOW('e', 'k', aes_key_t *)
#define SET_ENCRYPT _IO('e', 'e')
#define SET_DECRYPT _IO('e', 'd')
#define START_WRITE _IO('e', 'w')
#define START_READ _IO('e', 'r')
#define RESET _IO('e', 0)

#endif
