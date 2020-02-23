#!/bin/bash

usage()
{
	echo "Used:"
	echo "  $0 [project_name] [1.0.0] [target]"
}

if [ "$#" -lt 2 ]; then
	usage
	exit
fi

project_name=$1
user_sw_version="$2"
build_time="$(date +%H:%M:%S)"
build_date="$(date +%Y-%m-%d)"
extra_cflags="-DBUILD_TIME=\\\"$build_time\\\" -DBUILD_DATE=\\\"$build_date\\\" -DUSER_SW_VERSION=\\\"$user_sw_version\\\" -DPROJECT_NAME=\\\"$project_name\\\""

cd apps/$project_name
make EXTRA_CFLAGS="$extra_cflags" PROJECT_NAME="$project_name" $3

if [ "$?" -ne 0 ]; then
	echo "make error!"
	exit
fi

if [ ! -z $3 ]; then
	echo "$3 success!"
else
	echo "compile success!"
fi
