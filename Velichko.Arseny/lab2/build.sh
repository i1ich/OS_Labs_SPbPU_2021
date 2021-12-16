cmake -B build -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../bin
cd build || return
cmake --build . -j4

cd ..
rm -r build
