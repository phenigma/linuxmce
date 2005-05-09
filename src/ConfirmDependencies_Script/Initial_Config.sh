#!/bin/bash

# router/diskless
Type="router"

mkdir -p /var/log/pluto
File="/var/log/pluto/Initial_Install.newlog"
: >"$File"
exec 3>&1 4>&2
bash -x /usr/pluto/install/Initial_Config_Real.sh "$Type" > >(tee -a "$File" >&3) 2> >(tee -a "$File" | grep -v '^++* ' >&4)
sleep 1 # so tee and grep close and flush their buffers
exec 3>&- 4>&-
