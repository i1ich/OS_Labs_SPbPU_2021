#! /bin/bash

cmake -S src -B build

cd build
cmake --build .

mv source ../

cd ..

rm -r build
