pidPath="/var/run/lab1.pid"

[[ -f pidPath ]] || sudo touch "$pidPath"

sudo chmod 666 "$pidPath"

mkdir build
cd build
cmake -S ../ -B ./
make
cp ./lab1_var16 ../lab1_var16
cd ../
rm -r build/