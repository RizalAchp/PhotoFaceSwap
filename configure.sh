#!/bin/bash
export CC=$(which clang)
export CXX=$(which clang++)

cmake -B ./build -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE="Release" -S . -G "Ninja" -Wno-dev


unset CC
unset CXX
