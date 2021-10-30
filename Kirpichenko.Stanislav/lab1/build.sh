#! /bin/bash

pidDir="/var/run/kirpichenko_daemon"

if [ ! -d ${pidDir} ]
then
sudo mkdir ${pidDir}
fi

sudo chmod 777 ${pidDir}


cmake -S src -B build

cd build
cmake --build .

mv lab_daemon ../lab_daemon

cd ..

rm -r build
