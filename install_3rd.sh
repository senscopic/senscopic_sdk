#!/bin/bash
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH

#  install 3rd party 
echo "=============== start install 3rdparty resource ==============="

echo "install libudev-dev & libusb-1.0-0-dev"
sudo apt-get install libudev-dev libusb-1.0-0-dev

echo "add udev rule to /etc/udev/rules.d/"
cd ${BASEDIR}
sudo cp ./99-senscopic.rules /etc/udev/rules.d/

echo "start installing jsoncpp"
BASEDIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cd ${BASEDIR}/3rdparty/jsoncpp
rm CMakeCache.txt
cmake .
make -j4
echo "jsoncpp installed"


