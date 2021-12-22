mkdir build
# shellcheck disable=SC2164
cd build
cmake -S ../ -B ./
make

mv AsyncStructure ../

cd ../
rm -r build