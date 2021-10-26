#!/usr/bin/bash


bin_dir="bin"
name="daemon"


if [[ !(-z $1) && "$1" -eq "-r" ]]
then
    echo `rm -r $bin_dir`
else  
    echo `mkdir $bin_dir`
    echo `g++ -std=c++2a src/*.cpp -o $bin_dir/$name`
fi


