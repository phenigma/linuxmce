#!/bin/bash

# router/diskless
Type="router"

mkdir -p /var/log/pluto
bash -x /usr/pluto/install/Initial_Config_Real.sh "$Type" 1> >(tee /var/log/pluto/Initial_Install.newlog) 2>/var/log/pluto/Initial_Install_trace.newlog
