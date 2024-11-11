#!/bin/bash

# SPDX-FileCopyrightText: 2024 Integral <integral@member.fsf.org>
#
# SPDX-License-Identifier: GPL-2.0-only

if [ ! -e /dev/encrypt ]; then
    echo "/dev/encrypt does not exists!"
    exit 1
fi

echo "Building..."
make

echo
echo "Testing small data..."
./small_data

echo
echo "Testing large data..."

echo "Creating input data file..."
dd if=/dev/urandom of=2M_data_input bs=128K count=16

./large_data
echo -n "sha256sum of the input data: "
sha256sum 2M_data_input

if [ -f 2M_data_output ]; then
    echo -n "sha256sum of the output data: "
    sha256sum 2M_data_output
else
    echo "Encryption of large data failed!"
fi

echo
echo "Cleaning..."
rm -f 2M_data_input 2M_data_output
make clean
