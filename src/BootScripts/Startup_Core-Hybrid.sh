#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
export DISPLAY=:${Display}
while /bin/true ;do
	X_Is_Ok=true
	X_Start_Time=$(date +%s)
		/usr/pluto/bin/Start_X.sh -config /etc/X11/xorg.conf
		/usr/pluto/bin/lmce_launch_manager
	X_Stop_Time=$(date +%s)

	## If X exited to quick
	if [[ $(( $X_Stop_Time - $X_Start_Time )) -lt 4 ]] ;then
		X_Is_Ok=false
	fi

	## If X cannot start, run the script in console
	if [[ "$X_Is_Ok" == "false" ]] ;then
		echo "FAILED TO START X11"
	fi
done



