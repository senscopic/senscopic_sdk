#!/bin/bash
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH

BASEDIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
echo ${BASEDIR}

#  compile code
cd ${BASEDIR}
make clean
rm CMakeCache.txt
cmake CMakeLists.txt
cd ${BASEDIR}
make -j4
