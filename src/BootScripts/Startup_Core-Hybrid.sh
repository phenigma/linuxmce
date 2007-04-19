#!/bin/bash

X_Is_Ok=true

## Test to see if X exists
if [[ ! -x /usr/bin/xinit ]] ;then
	X_Is_Ok=false
fi

if [[ "$X_Is_Ok" == "true" ]] ;then
	X_Start_Time=$(date +%s)
	xinit /usr/bin/mce-launcher -- :3 -config /usr/share/mce-launcher/mce-launcher.xorg.conf -br
	X_Stop_Time=$(date +%s)

	## If X exited to quick
	if [[ $(( $X_Stop_Time - $X_Start_Time )) -lt 4 ]] ;then
		X_Is_Ok=false
	fi
fi

## If X cannot start, run the script in console
if [[ "$X_Is_Ok" == "false" ]] ;then
	/usr/share/mce-launcher/scripts/mce-launcher-core.sh
fi


