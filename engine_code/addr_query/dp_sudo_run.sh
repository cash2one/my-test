#!/bin/bash

filepath=$(cd "$(dirname "$0")"; pwd)
cd $filepath
killall -9 ip_query_watch.sh
chmod +x ${filepath}/ip_query_watch.sh
rm -rf ${filepath}/nohup.out
nohup ${filepath}/ip_query_watch.sh >/dev/null &
cd -
