# Linux Kernel Lab
Integral's Linux Kernel Lab

## Lab 1: An AES256 encryption driver for character devices
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
