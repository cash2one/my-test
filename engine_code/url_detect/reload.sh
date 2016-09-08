#!/bin/bash

kill -HUP `ps a|grep url_detect.py|grep -v grep|awk '{print $1}'`
