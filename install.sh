#!/bin/bash
sudo apt-get -y install git make build-essential qt5-default qt5-qmake
git clone https://github.com/jedisct1/libsodium.git
git clone https://github.com/yvbbrjdr/pipesocks.git
cd libsodium/
git checkout stable
./configure
make && sudo make install
sudo cp /usr/local/lib/libsodium.so.18 /usr/lib/
cd ../pipesocks/pipesocks/
git checkout stable
qmake && make
cp pipesocks ../../libsodium/
cd ../../
sudo rm -R pipesocks/
cp libsodium/pipesocks ./
sudo rm -R libsodium/
