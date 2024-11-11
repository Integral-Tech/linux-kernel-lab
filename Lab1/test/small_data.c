// SPDX-FileCopyrightText: 2024 Integral <integral@member.fsf.org>
//
// SPDX-License-Identifier: GPL-2.0-only

#include "cdev_ioctl.h"
#include "cleanup.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>

#define CDEV_PATH "/dev/encrypt"

int main()
{
	__attribute__((cleanup(fd_close))) int fd = open(CDEV_PATH, O_RDWR);
	if (fd < 0) {
		printf("Failed to open device %s\n", CDEV_PATH);
		return -1;
	}

	aes256_key_t aes256_key = {
		.key = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
			 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f }
	};
	ioctl(fd, SET_KEY, &aes256_key);
	ioctl(fd, SET_ENCRYPT);
	ioctl(fd, START_WRITE);

	uint8_t in[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
			 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

	write(fd, in, sizeof(in));

	fd = open(CDEV_PATH, O_RDWR);
	if (fd < 0) {
		printf("Failed to open device %s\n", CDEV_PATH);
		return -1;
	}

	ioctl(fd, SET_DECRYPT);
	ioctl(fd, START_READ);

	__attribute__((cleanup(free_ptr))) uint8_t *out = malloc(sizeof(in));
	if (read(fd, out, sizeof(in)) < 0) {
		printf("An error occurred while reading from device %s\n",
		       CDEV_PATH);
		return -1;
	}

	printf("Successed to read from device %s, fd = %d\n", CDEV_PATH, fd);
	ioctl(fd, RESET);

	puts("Input data:");
	for (size_t i = 0; i < sizeof(in) / sizeof(uint8_t); i++)
		printf("%02x ", in[i]);

	puts("\nOutput data:");
	const uint8_t *ptr = out;
	for (size_t i = 0; i < sizeof(in) / sizeof(uint8_t); i++, ptr++)
		printf("%02x ", *ptr);

	putchar('\n');
	return 0;
}
