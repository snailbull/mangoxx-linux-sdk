#!/bin/bash
set -e

SDK_PATH=../..

echo "make_lib.sh version 20200220"
echo ""

for dir in crc cJSON mango minilzo; do
    cd $dir
    make clean
    make
    cp .output/debug/lib/lib$dir.a $SDK_PATH/lib/lib$dir.a
    strip --strip-unneeded $SDK_PATH/lib/lib$dir.a
    cd ..
done;