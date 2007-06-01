#!/bin/bash

invoke-rc.d mysql start

. /usr/pluto/bin/Config_Ops.sh
export DISPLAY=:${Display}
while /bin/true ;do
	XPID=$(</var/run/plutoX$Display.pid)
	if [[ -z "$XPID" || ! -d /proc/"$XPID" ]] ;then
		/usr/pluto/bin/Start_X.sh -config /etc/X11/xorg.conf
	fi

	export KDE_DEBUG=1
	/usr/pluto/bin/lmce_launch_manager

	while [[ "$(pidof lmce_launch_manager)" != "" ]] ;do
		sleep 5
	done
	export -n KDE_DEBUG
done



