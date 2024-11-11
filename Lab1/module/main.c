// SPDX-FileCopyrightText: 2024 Integral <integral@member.fsf.org>
//
// SPDX-License-Identifier: GPL-2.0-only

#include "aes/aes.h"
#include "cdev_ioctl.h"
#include <linux/vmalloc.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/string.h>

static dev_t dev;
static struct cdev *cdev;
static unsigned dev_count = 1;

static aes_key_t aes_key;
static bool mode, read_flag = false, write_flag = false;
static uint8_t *cdev_buf = NULL;

static const uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

static int cdev_open(struct inode *inode, struct file *file)
{
	int major = MAJOR(inode->i_rdev), minor = MINOR(inode->i_rdev);
	printk("%s: major number = %d, minor number = %d\n", __func__, major,
	       minor);

	return 0;
}

static int cdev_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t cdev_read(struct file *file, char __user *buf, size_t size,
			 loff_t *loff)
{
	if (!read_flag) {
		pr_err("Device is not ready for reading!\n");
		return -1;
	}

	if (!cdev_buf || !buf || !loff) {
		pr_err("%s: Invalid arguments!\n", __func__);
		return -EINVAL;
	}

	uint8_t *cipher_buf = vmalloc(size);
	if (!cipher_buf) {
		pr_err("Failed to allocate memory for the buffer!\n");
		return -ENOMEM;
	}

	memcpy(cipher_buf, cdev_buf + *loff, size);
	aes_ctx_t ctx;
	aes_init_ctx_iv(&ctx, aes_key.key, iv);
	aes_cbc_decrypt_buffer(&ctx, cipher_buf, size);

	size_t unread = copy_to_user(buf, cipher_buf, size);
	size_t read = size - unread;
	if (unread)
		pr_err("Failed to read all data to userspace!");

	if (!read) {
		pr_err("Failed to read data to userspace!");
		goto read_failed;
	}

	*loff += read;
	printk("Succeeded to read %lu bytes data\n", read);
	printk("Current position: %lld\n", *loff);
	vfree(cipher_buf);
	return read;

read_failed:
	vfree(cipher_buf);
	return 0; // Read 0 bytes of data
}

static ssize_t cdev_write(struct file *file, const char __user *buf,
			  size_t size, loff_t *loff)
{
	if (!write_flag) {
		pr_err("Device is not ready for writing!\n");
		return -1;
	}

	if (!buf || !loff) {
		pr_err("%s: Invalid arguments!\n", __func__);
		return -EINVAL;
	}

	const bool expand = cdev_buf;
	if (expand)
		cdev_buf =
			krealloc(cdev_buf, ksize(cdev_buf) + size, GFP_KERNEL);
	else
		cdev_buf = kmalloc(size, GFP_KERNEL);

	if (!cdev_buf) {
		pr_err("Failed to allocate memory for the buffer!\n");
		return -ENOMEM;
	}

	uint8_t *cipher_buf = vmalloc(size);
	if (!cipher_buf) {
		pr_err("Failed to allocate memory for the buffer!\n");
		goto write_failed;
	}

	size_t unwritten = copy_from_user(cipher_buf, buf, size);
	size_t written = size - unwritten;
	if (unwritten)
		pr_err("Failed to copy all data from userspace!\n");

	if (!written) {
		pr_err("Failed to copy data to userspace!");
		goto write_failed;
	}

	aes_ctx_t ctx;
	aes_init_ctx_iv(&ctx, aes_key.key, iv);
	aes_cbc_encrypt_buffer(&ctx, cipher_buf, size);

	memcpy(cdev_buf + *loff, cipher_buf, size);
	*loff += written;
	printk("Succeeded to write %lu bytes data\n", written);
	printk("Current position: %lld\n", *loff);

	vfree(cipher_buf);
	return written;

write_failed:
	if (cipher_buf)
		vfree(cipher_buf);

	if (expand)
		cdev_buf = krealloc(cdev_buf, ksize(cdev_buf), GFP_KERNEL);
	else
		kfree(cdev_buf);
	return -1;
}

static long cdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	aes_key_t aes_key;

	switch (cmd) {
	case SET_KEY:
		if (copy_from_user(&aes_key, (aes_key_t *)arg,
				   sizeof(aes_key))) {
			pr_err("Failed to set AES256 key!\n");
			return -1;
		}

		break;
	case SET_ENCRYPT:
		mode |= ENCRYPT;
		break;

	case SET_DECRYPT:
		mode &= DECRYPT;
		break;

	case START_WRITE:
		write_flag |= true;
		break;

	case START_READ:
		read_flag |= true;
		break;

	case RESET:
		memset(cdev_buf, 0, ksize(cdev_buf));
		break;
	}

	return 0;
}

static const struct file_operations cdev_operations = {
	.owner = THIS_MODULE,
	.open = cdev_open,
	.release = cdev_release,
	.read = cdev_read,
	.write = cdev_write,
	.unlocked_ioctl = cdev_ioctl,
};

static int __init module_init_entry(void)
{
	int ret = alloc_chrdev_region(&dev, 0, dev_count, CDEV_NAME);
	if (ret) {
		printk("Failed to allocate region for a new character device!\n");
		return ret;
	}

	cdev = cdev_alloc();
	if (!cdev) {
		printk("Failed to allocate a cdev structure!\n");
		goto alloc_failed;
	}

	cdev_init(cdev, &cdev_operations);
	ret = cdev_add(cdev, dev, dev_count);
	if (ret) {
		printk("Failed to add the character device to system!\n");
		goto add_failed;
	}

	printk("Succeeded to create a character device: %s\n", CDEV_NAME);
	printk("Major number = %d, minor number = %d\n", MAJOR(dev),
	       MINOR(dev));
	return 0;

add_failed:
	cdev_del(cdev);
alloc_failed:
	unregister_chrdev_region(dev, dev_count);
	return ret;
}

static void __exit module_exit_entry(void)
{
	printk("Removing the character device...\n");
	if (cdev)
		cdev_del(cdev);

	unregister_chrdev_region(dev, dev_count);
}

module_init(module_init_entry);
module_exit(module_exit_entry);

MODULE_AUTHOR("Integral");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("An AES256 encryption driver for character devices");
