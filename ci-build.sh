#!/bin/bash

mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH=cmake-package -DCMAKE_BUILD_TYPE="$1" ..
make
make install
