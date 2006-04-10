#!/bin/bash

## Make sure the had / dbus-1 are started
/etc/init.d/dbus-1 start

## Start our daemon in a screen
screen -d -m -S "PlutoHalDaemon.sh" /usr/pluto/bin/pluto-hald dcerouter

while [[ "1" == "1" ]];do

	sleep 10

	pidHal=$(pidof hald)
	if [[ $pidHal == "" ]];then
		/etc/init.d/dbus-1 restart
	fi
	

	pidPlutoHal=$(pidof pluto-hald)
	if [[ $pidPlutoHal == "" ]]; then
		screen -d -m -S "PlutoHalDaemon.sh" /usr/pluto/bin/pluto-hald dcerouter
	fi
done
