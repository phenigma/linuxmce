#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

cd /usr/pluto/bin
echo "Starting Orbiter"

export DISPLAY=:0
screen -d -m -S Orbiter bash -c "(echo \$(date) Starting; /usr/pluto/bin/Orbiter -d 1 -r localhost) | tee Orbiter.log"
sleep 1
