PID_DIRECTORY="/var/run/user_daemon/"
if [ ! -d ${PID_DIRECTORY} ]
then
sudo mkdir ${PID_DIRECTORY}
fi
sudo chmod 777 ${PID_DIRECTORY}

cmake -S src -B build -DPID_DIRECTORY=${PID_DIRECTORY}
cd build || return
cmake --build .

mv Daemon ../Daemon
cd ..
rm -r build