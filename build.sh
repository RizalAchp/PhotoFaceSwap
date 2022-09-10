#!/bin/bash
set -xe

cmake --build ./build --target all -- -j4
