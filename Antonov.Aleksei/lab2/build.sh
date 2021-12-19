mkdir build
# shellcheck disable=SC2164
cd build
cmake -S ../ -B ./
make

mv Wolf_Goat_Game* ../

cd ../
rm -r build