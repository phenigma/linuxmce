#!/bin/bash

if [[ -f /etc/diskless.conf ]] ;then
	exit 0
fi

if [[ ! -L /usr/pluto/orbiter/floorplans && -d /usr/pluto/orbiter/floorplans ]]; then
	mv /usr/pluto/orbiter/floorplans /home/pluto/
	ln -sn /home/pluto/floorplans /usr/pluto/orbiter/floorplans
fi
