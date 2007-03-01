#!/bin/bash

if [[ -f /etc/diskless.conf ]] ;then
	exit 0
fi

if [[ ! -L /usr/pluto/orbiter/floorplans && -d /usr/pluto/orbiter/floorplans ]]; then
	if [[ ! -d /home/pluto/floorplans ]]; then
		mkdir -p /home/pluto/floorplans
	fi
	mv /usr/pluto/orbiter/floorplans/* /home/pluto/floorplans/
	rm -rf /usr/pluto/orbiter/floorplans
	ln -sn /home/pluto/floorplans /usr/pluto/orbiter/floorplans
fi
