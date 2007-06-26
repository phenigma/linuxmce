#!/bin/bash

set -x
exec >>/var/log/pluto/RestartLocalX.log 2>&1
date -R

if [[ -f /etc/event.d/pluto || "$PK_Distro" == 1 ]]; then
	kill $(pidof X)
	if [[ "$PK_Distro" == 1 ]]; then
		sleep 1
		/usr/pluto/bin/Start_X.sh
	fi
else
	export TERM=linux
	/etc/init.d/kdm restart &>/dev/null </dev/null
	echo "kdm restart exit code: $?"
fi
