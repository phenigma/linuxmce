#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

cd /usr/pluto/bin
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting Orbiter"

export DISPLAY=:0

#echo launching out wrapper to launch Mythtv setup if not configured

./MythTvConfiguredCheck.sh

screen -d -m -S Orbiter bash -c "(echo \$(date) Starting; /usr/pluto/bin/Orbiter -d 1 -r localhost) | tee Orbiter.log"
sleep 1
