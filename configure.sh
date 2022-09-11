#!/bin/bash
export C=$(which clang)
export CXX=$(which clang++)

cmake -B ./build -DCMAKE_BUILD_TYPE="Release" -S . -G "Ninja"


unset C
unset CXX
