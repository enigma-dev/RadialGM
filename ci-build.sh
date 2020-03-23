#!/bin/bash

mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="$1" -DCMAKE_INSTALL_PREFIX:PATH="$2" ..
VERBOSE=1 make
make install
