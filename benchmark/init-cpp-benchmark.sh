#!/bin/bash

rm -rf libdatachannel
git clone --recursive https://github.com/paullouisageneau/libdatachannel.git
cd cppbenchmark
cppdir=`pwd`
cd ..
ln -s $cppdir libdatachannel/examples
cd libdatachannel
echo "add_subdirectory(examples/cppbenchmark)" >> CMakeLists.txt
cmake -B build -DUSE_GNUTLS=1 -DUSE_NICE=0 -DCMAKE_BUILD_TYPE=Release
cd build
make -j8
