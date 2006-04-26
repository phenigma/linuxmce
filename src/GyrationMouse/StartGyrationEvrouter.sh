#!/bin/bash

function LogMessage {
	echo $(date -R) $*
}

if [[ "$(pidof evrouter)" != "" ]]; then
	LogMessage "Evrouter allready running, exiting"
	exit 0
fi

LogMessage "Removing everouter lock file"
rm -f /tmp/.evrouter*

LogMessage "Waiting for Xorg to start"
couter=0
while [[ "$(ps -A | grep Xorg | wc -l)" == "0" && "$counter" != "20" ]]; do
	echo  -n "."
	sleep 0.5
	counter=$(( $counter + 1 ))
done
echo

if [[ "$counter" == "20" ]] ;then
	LogMessage "Xorg faild to start, extiting"
	exit 0
fi

if [[ ! -h /dev/input/gyration ]] ;then
	LogMessage "Device /dev/input/gyration not found, exiting"
	exit 0
fi

LogMessage "Starting evrouter for /dev/input/gyration"
export DISPLAY=:0 
/usr/bin/evrouter /dev/input/gyration

