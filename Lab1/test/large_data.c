// SPDX-FileCopyrightText: 2024 Integral <integral@member.fsf.org>
//
// SPDX-License-Identifier: GPL-2.0-only

#include "cdev_ioctl.h"
#include "cleanup.h"
#include <fcntl.h>
#include <string.h>
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

	__attribute__((cleanup(file_close)))
	FILE *input_file = fopen("2M_data_input", "rb");
	fseek(input_file, 0, SEEK_END);
	size_t file_size = ftell(input_file);
	fseek(input_file, 0, SEEK_SET);

	__attribute__((cleanup(free_ptr))) uint8_t *buffer = malloc(file_size);
	fread(buffer, sizeof(uint8_t), file_size, input_file);

	write(fd, buffer, file_size);

	fd = open(CDEV_PATH, O_RDWR);
	if (fd < 0) {
		printf("Failed to open device %s\n", CDEV_PATH);
		return -1;
	}

	ioctl(fd, SET_DECRYPT);
	ioctl(fd, START_READ);
	memset(buffer, 0, file_size);
	if (read(fd, buffer, file_size) < 0) {
		printf("An error occurred while reading from device %s\n",
		       CDEV_PATH);
		return -1;
	}

	printf("Successed to read from device %s, fd = %d\n", CDEV_PATH, fd);
	ioctl(fd, RESET);

	__attribute__((cleanup(file_close)))
	FILE *output_file = fopen("2M_data_output", "wb");
	fwrite(buffer, sizeof(uint8_t), file_size, output_file);

	return 0;
}
