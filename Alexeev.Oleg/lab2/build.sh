#!/bin/bash

mkdir build
cmake -S ./ -B build
cd build

cmake --build
make
mv host* ../
mv client* ../

cd ../
rm -R build
