#!/bin/bash

pidFilePath="/var/run/lab1.pid"

if ! [[ -f pidFilePath ]]; then
  sudo touch "$pidFilePath"
fi

sudo chmod 666 "$pidFilePath"

cmake .
make
rm CMakeCache.txt Makefile cmake_install.cmake
FILE=lab1.cbp
if [[ -f "$FILE" ]]; then
    rm "$FILE"
fi
rm -r CMakeFiles/