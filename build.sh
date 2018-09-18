#!/bin/bash

mkdir -p ~/dg-jsoncpp
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DCMAKE_INSTALL_PREFIX=~/dg-jsoncpp -DCMAKE_INSTALL_INCLUDEDIR=include/jsoncpp -DBUILD_SHARED_LIBS=ON -DARCHIVE_INSTALL_DIR=. -G "Unix Makefiles" ../..
make && make install

home_dir=`echo ~`

if [ ! -d "$home_dir/bak-sys-jsoncpp" ];then
    echo "--------bakup system libjsoncpp-------"
    mkdir -p ~/bak-sys-jsoncpp/lib
    mkdir -p ~/bak-sys-jsoncpp/include
    sudo mv /usr/lib/x86_64-linux-gnu/libjsoncpp* ~/bak-sys-jsoncpp/lib
    sudo mv /usr/include/jsoncpp ~/bak-sys-jsoncpp/include
fi
echo "install dgjsoncpp to $home_dir/dg-jsoncpp"

ln -s -f ~/dg-jsoncpp/lib/libjsoncpp.so ~/dg-jsoncpp/lib/libjsoncpp.so.1
sudo cp ~/dg-jsoncpp/lib/libjsoncpp* /usr/lib/x86_64-linux-gnu
sudo cp -r ~/dg-jsoncpp/include/jsoncpp /usr/include/
