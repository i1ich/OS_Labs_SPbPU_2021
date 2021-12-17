#!/bin/bash

echo `cmake -S ./ -B ./bin_tmp`
echo `cmake --build ./bin_tmp`
echo `mkdir ./bin`
echo `mv ./bin_tmp/mipc ./bin/mipc`
echo `rm -r ./bin_tmp`