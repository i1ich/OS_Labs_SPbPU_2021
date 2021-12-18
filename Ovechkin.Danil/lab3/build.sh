#!/bin/sh

mkdir "build"

cmake -S ./ -B build

cd build
cmake --build .


