#!/bin/bash

Usage()
{
	echo "Syntax: $1 <IP>"
}

MakeChanges()
{
	shopt -s nullglob

	sed_cmd='s/^EnableGenerator=.*$/EnableGenerator=1/; s/^DelayMin=.*$/DelayMin=500/; s/^DelayMax=.*$/DelayMax=700/'
	sed -i "$sed_cmd" /etc/Orbiter.conf
	for i in /home/diskless/* 2>/dev/null; do
		sed -i "$sed_cmd" "$i"/etc/Orbiter.conf
	done
	/usr/pluto/bin/MessageSend localhost 0 -1000 7 1
}

Core="$1"

if [[ -z "$Core" ]]; then
	Usage "$0"
	exit
fi

if [[ "$Core" == local ]]; then
	MakeChanges
else
	cat "$0" | ssh root@"$Core" 'cat >/home/update_simulator.sh; bash -c "/home/update_simulator.sh local"' || echo "Failed"
fi
