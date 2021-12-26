#!/bin/sh

mkdir "build"

cmake -S ./ -B build

cd build

cmake --build .

mv lab3 ../

cd ..

rm -r build