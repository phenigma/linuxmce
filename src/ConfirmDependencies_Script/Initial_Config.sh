#!/bin/bash

# router/diskless
Type="router"

bash -x /usr/pluto/install/Initial_Config_Real.sh "$Type" &> >(tee /var/log/pluto/Initial_Install.newlog)
