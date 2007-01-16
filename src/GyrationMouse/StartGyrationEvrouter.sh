#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh 2>/dev/null || exit 1  ## So the orbiter can get the environment variable for the mysql server

function LogMsg {
	echo $(date -R) $*
}


while true; do

if [[ ! -h /dev/input/gyration ]] ;then
	LogMsg "Device /dev/input/gyration not found, sleeping for 5 seconds"
	sleep 5s
fi

if [[ "$(pidof X)" == "" ]]; then
	LogMsg "Xorg not found running, sleeping for 5 seconds"
	sleep 5s
fi

if [[ -h /dev/input/gyration ]] && [[ "$(pidof X)" != "" ]] && [[ "$(pidof evrouter)" == "" ]]; then
	LogMsg "Removing everouter lock file"
	rm -f /tmp/.evrouter*
	LogMsg "Starting evrouter for /dev/input/gyration"
	export DISPLAY=:$Display
	/usr/bin/evrouter /dev/input/gyration
	sleep 5s
fi 

done
