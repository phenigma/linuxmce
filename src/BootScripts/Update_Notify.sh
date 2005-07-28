#!/bin/bash

shopt -s nullglob

Lock=/usr/pluto/locks/upgrade_ok
Files=/var/cache/apt/archives/*
if [[ -f "$Lock" && -n "$Files" ]]; then
	echo "$(date -R) New packages ($(<"$Lock"))" >>/var/log/pluto/upgrade.newlog
	/usr/pluto/bin/MessageSend localhost 0 -1001 2 47
fi
