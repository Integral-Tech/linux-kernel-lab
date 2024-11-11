<!--
SPDX-FileCopyrightText: 2024 Integral <integral@member.fsf.org>

SPDX-License-Identifier: GPL-2.0-only
-->

# Linux Kernel Lab
Integral's Linux Kernel Lab

## Lab 1: An AES256 encryption driver for character devices
- Thanks to [kokke/tiny-AES-c](https://github.com/kokke/tiny-AES-c)

### How to build?

```bash
git clone https://github.com/Integral-Tech/linux-kernel-lab.git
cd linux-kernel-lab/Lab1/module/
make
```

### How to install?
Execute the following commands in ```module``` directory:

```bash
sudo insmod encrypt.ko
sudo mknod /dev/encrypt c $(cat /proc/devices | grep encrypt | awk '{print $1}') 0
```

### How to test?
Execute the following commands in ```test``` directory:

```bash
sudo ./test.sh
```

## License
- Licensed under the [GNU General Public License, version 2](LICENSES/GPL-2.0-only.txt).
