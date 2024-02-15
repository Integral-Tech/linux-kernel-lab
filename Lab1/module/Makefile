# SPDX-FileCopyrightText: 2024 Integral <integral@member.fsf.org>
#
# SPDX-License-Identifier: GPL-2.0-only

obj-m := encrypt.o
encrypt-objs := main.o aes/aes.o
KDIR := /lib/modules/`uname -r`/build/

.PHONY: all clean
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
