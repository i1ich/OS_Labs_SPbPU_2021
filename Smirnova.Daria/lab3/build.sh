mkdir build
cd build
cmake -S ../ -B ./
make

mv lab3_var2 ../

cd ../
rm -r build