wget https://github.com/accellera-official/systemc/archive/refs/tags/3.0.1.tar.gz
tar zxfv 3.0.1.tar.gz
cd systemc-3.0.1/
mkdir build && cd build
cmake ../ -DCMAKE_CXX_STANDARD=11 -DCMAKE_BUILD_TYPE=Debug
make -j 8
