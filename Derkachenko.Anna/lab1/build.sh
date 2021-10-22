#!/usr/bin/env bash

mkdir build
cd build || exit
cmake ..
cmake --build .
