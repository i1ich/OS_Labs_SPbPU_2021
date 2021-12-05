#! /bin/bash

conn_files=$(find ./src -name *.cpp -printf "%f\n" | grep "conn_*")
connections=$(echo $conn_files | sed 's/.cpp//g' | sed 's/conn_//g')

cmake -S src -B build -D INTERACTION_TYPES="$connections"

cd build
cmake --build .

mv host* ../

cd ..

rm -r build
