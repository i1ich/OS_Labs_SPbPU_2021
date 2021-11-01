#!/bin/bash

pidFilePath="/var/run/lab1.pid"

[[ -f $pidFilePath ]] || sudo touch "$pidFilePath"

sudo chmod 666 "$pidFilePath"

cmake .
make
rm CMakeCache.txt Makefile cmake_install.cmake
FILE=daemon_lab.cbp
[[ -f "$FILE" ]] && rm "$FILE"
rm -r CMakeFiles/
