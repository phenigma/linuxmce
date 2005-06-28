#!/bin/bash

Answer="$1"

pidOfX="$(ps ax|grep 'X :1 -ac -xf86config /etc/X11/XF86Config-4.test'|grep -v grep|cut -f1 -d' ')"
if [[ -z "$pidOfX" ]]; then
	echo "test X not running. nothing to kill"
fi
if [[ ! -f "/etc/X11/XF86Config-4.test" ]]; then
	echo "test config not found. no new settings to apply"
	exit 1
fi

echo "Killing test X"
kill $pidOfX

if [[ "${Answer:0:1}" == "y" || "${Answer:0:1}" == "Y" ]]; then
	pidOfX=$(ps ax|grep 'X -ac -allowMouseOpenFail vt7'|grep -v grep|cut -f1 -d' ')
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
