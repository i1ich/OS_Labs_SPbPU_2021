#!/bin/sh

mkdir "build"

cmake -S ./ -B build -D CONN_TYPES="$1"

cd build

cmake --build .

mv host* ../
mv client* ../

cd ..

rm -r build

