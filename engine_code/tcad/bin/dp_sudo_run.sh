#!/bin/bash

filepath=$(cd "$(dirname "$0")"; pwd)

killall -9 watchdog_dp.sh
chmod +x ${filepath}/watchdog_dp.sh
rm -rf ${filepath}/nohup.out
nohup ${filepath}/watchdog_dp.sh >/dev/null &
