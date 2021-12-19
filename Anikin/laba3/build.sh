#!/bin/bash

echo `cmake -S ./ -B ./bin_tmp`
echo `cmake --build ./bin_tmp`
echo `mkdir ./bin`
echo `mv ./bin_tmp/lock_free_struct ./bin/lock_free_struct`
echo `rm -r ./bin_tmp`