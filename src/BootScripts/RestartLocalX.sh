#!/bin/bash
if [[ -f /etc/event.d/pluto || "$PK_Distro" == 1 ]]; then
	kill $(pidof X)
	if [[ "$PK_Distro" == 1 ]]; then
		sleep 1
		/usr/pluto/bin/Start_X.sh
	fi
else
	export TERM=linux
	/etc/init.d/kdm restart &>/dev/null </dev/null &
	disown -a
fi
