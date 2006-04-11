#!/bin/bash

## If run in background
if [[ "$1" == "background" ]]; then

	while [[ "1" == "1" ]];do
		pidHal=$(pidof hald)
		if [[ $pidHal == "" ]];then
			/etc/init.d/dbus-1 start
		fi

		pidPlutoHal=$(pidof pluto-hald)
		if [[ $pidPlutoHal == "" ]]; then
			/usr/pluto/bin/pluto-hald dcerouter
		fi
	done

## If not run in background
else
	## Start this script in background
	screen -d -m -S "PlutoHalDaemon.sh" /usr/pluto/bin/PlutoHalDaemon.sh background
fi
