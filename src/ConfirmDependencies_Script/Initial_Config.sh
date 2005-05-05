#!/bin/bash

# router/diskless
Type="router"

mkdir -p /var/log/pluto
bash -x /usr/pluto/install/Initial_Config_Real.sh "$Type" &> >(tee /var/log/pluto/Initial_Install.newlog | grep -v '^++* ')
sleep 1 # so tee and grep close and flush their buffers
