#!/bin/bash

mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="$1" ..
make
make install
