rm -rf libdatachannel
git clone --recursive https://github.com/paullouisageneau/libdatachannel.git
ln -s cppbenchmark libdatachannel/examples
cd libdatachannel
echo "add_subdirectory(examples/cppbenchmark)" >> CMakeLists.txt
cmake -B build -DUSE_GNUTLS=1 -DUSE_NICE=0
cd build
make -j8
