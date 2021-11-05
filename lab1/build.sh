#!/bin/sh

PID_LOCATION="/var/run/daemon.pid"

[[ -f $pidFilePath ]] || sudo touch "$PID_LOCATION"

sudo chmod 666 "$PID_LOCATION"

cmake -Wall -Werror .
make
rm CMakeCache.txt Makefile cmake_install.cmake
FILE=lab1.cbp
[[ -f "$FILE" ]] && rm "$FILE"
rm -r CMakeFiles/
