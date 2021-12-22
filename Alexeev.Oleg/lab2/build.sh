#!/bin/bash

mkdir build
cmake -S ./ -B build
cd build

cmake --build
make
mv host_* ../

cd ../
rm -R build
