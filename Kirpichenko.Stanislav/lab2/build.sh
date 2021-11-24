#! /bin/bash

cmake -S src -B build

cd build
cmake --build .

mv host* ../

cd ..

rm -r build
