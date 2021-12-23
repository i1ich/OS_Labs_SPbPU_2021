cmake -B build -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../bin
cd build || return
cmake --build ./

cd ..
rm -r build
