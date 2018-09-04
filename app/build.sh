#!/bin/bash

usage()
{
	echo "Used:"
	echo "  $0 [1.0.0] [project_name]"
}

if [ "$#" -ne 3 ]; then
	usage
	exit
fi

user_sw_version="$1"
proj_name=$2
build_time="$(date +%H:%M:%S)"
build_date="$(date +%Y-%m-%d)"
extra_ccflags="-DGLOBAL_DEBUG -DBUILD_TIME=\\\"$build_time\\\" -DBUILD_DATE=\\\"$build_date\\\" -DUSER_SW_VERSION=\\\"$user_sw_version\\\" -DPROJ_NAME=\\\"$proj_name\\\""

make EXTRA_CCFLAGS="$extra_ccflags"

if [ "$?" -ne 0 ]; then
	echo "make error!"
	exit
fi
echo "compile ok!"
