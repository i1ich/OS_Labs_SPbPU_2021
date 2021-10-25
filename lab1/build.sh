#! /bin/bash

pidFile="/var/run/IgnatievDeamon.pid"

[[ -f pidFile ]] || sudo touch "$pidFile"

sudo chmod 0666 "$pidFile"

make
