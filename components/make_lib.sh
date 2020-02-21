#!/bin/bash
set -e

SDK_PATH=../..

echo "make_lib.sh version 20200220"
echo ""

cd $1
make clean
make
cp .output/debug/lib/lib$1.a $SDK_PATH/lib/lib$1.a
strip --strip-unneeded $SDK_PATH/lib/lib$1.a
cd ..
