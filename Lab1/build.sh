pidPath="/var/run/lab1_var16.pid"

[[ -f pidPath ]] || sudo touch "$pidPath"

sudo chmod 666 "$pidPath"

mkdir build
cd build
cmake -S ../ -B ./
make
cp ./lab1_16 ../lab1_16
cd ../
rm -r build/
