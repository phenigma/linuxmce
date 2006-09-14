#!/bin/bash

function XorgConfLogging() {
        local message="$1"
        local xorgLog="/var/log/pluto/xorg.conf.log"
        local xorgLines=$(cat /etc/X11/xorg.conf | wc -l)
        local myPid=$$

        echo "$myPid $(date -R) $message [$xorgLines]"  >> $xorgLog
}

XorgConfLogging "Starting $0 $*"
trap 'XorgConfLogging "Ending"' EXIT

Answer="$1"

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

pidOfX="$(ps ax|grep 'X :1 -ignoreABI -ac -config /etc/X11/xorg.conf.test'|grep -v grep|awk '{print $1}')"
if [[ -z "$pidOfX" ]]; then
	echo "test X not running. nothing to kill"
fi
if [[ ! -f "/etc/X11/xorg.conf.test" ]]; then
	echo "test config not found. no new settings to apply"
	exit 10
fi

echo "Killing test X"
kill $pidOfX

if [[ "${Answer:0:1}" == "y" || "${Answer:0:1}" == "Y" ]]; then
	echo "X reload flag and message"
	ReloadDevicesOnThisMachine
	
	pidOfX=$(ps ax|grep 'X :0 -ignoreABI -ac -allowMouseOpenFail vt7'|egrep -v 'grep|SCREEN'|awk '{print $1}')
	mv /etc/X11/xorg.conf{.test,}
	echo "Killing real X (to restart it with new config)"
	kill $pidOfX
	sleep 5
	/usr/pluto/bin/Start_X.sh
else
	rm /etc/X11/xorg.conf.test
fi
echo
