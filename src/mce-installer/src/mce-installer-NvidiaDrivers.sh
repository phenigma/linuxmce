#!/bin/bash

sleep 2
#apt-get install nvidia-glx
#apt-get install linux-restricted-modules-generic
sed -i 's/Driver.*"nv"/Driver "nvidia"/g' /etc/X11/xorg.conf
