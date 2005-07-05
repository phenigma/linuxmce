#!/bin/bash

Answer="$1"

. /usr/pluto/bin/Config_Ops.sh

TellToReload()
{
	for Dev in $(cat /usr/pluto/locks/pluto_spawned_local_devices.txt); do
		/usr/pluto/bin/MessageSend "$DCERouter" 0 "$Dev" 7 1
	done
}

pidOfX="$(ps ax|grep 'X :1 -ac -xf86config /etc/X11/XF86Config-4.test'|grep -v grep|awk '{print $1}')"
if [[ -z "$pidOfX" ]]; then
	echo "test X not running. nothing to kill"
fi
if [[ ! -f "/etc/X11/XF86Config-4.test" ]]; then
	echo "test config not found. no new settings to apply"
	exit 10
fi

echo "Killing test X"
kill $pidOfX

if [[ "${Answer:0:1}" == "y" || "${Answer:0:1}" == "Y" ]]; then
	echo "X reload flag and message"
	TellToReload
	
	pidOfX=$(ps ax|grep 'X -ac -allowMouseOpenFail vt7'|egrep -v 'grep|SCREEN'|awk '{print $1}')
	mv /etc/X11/XF86Config-4{.test,}
	echo "Killing real X (to restart it with new config)"
	kill $pidOfX
	sleep 5
	/usr/pluto/bin/Start_X.sh
	/usr/pluto/bin/Start_ratpoison.sh
else
	rm /etc/X11/XF86Config-4.test
fi
echo
