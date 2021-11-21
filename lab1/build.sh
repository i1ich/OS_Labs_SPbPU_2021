pidPath="/var/run/lab1.pid"

[[ -f pidPath ]] || sudo touch "$pidPath"

sudo chmod 666 "$pidPath"

mkdir build
cd build
cmake .. 
make
cp ./lab1 ../lab1
cd ../
rm -r build/
