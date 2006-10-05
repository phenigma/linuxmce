#!/bin/bash

function DisplayUsage() {
	echo "Usage: RestartX.sh <orbiterID> <computerIP>"
}

if [[ "$#" != "2" ]] ;then
	DisplayUsage
	exit 1	
fi

orbiterID=$1
computerIP=$2

/usr/pluto/bin/LanchRemoteCmd.sh "$computerIP" "
	. /usr/pluto/bin/Utils.sh;
	kill $(ps ax|grep 'X :0 -ignoreABI -ac -allowMouseOpenFail vt7'|egrep -v 'grep|SCREEN'|awk '{print $1}')
	sleep 5;
	/usr/pluto/bin/Start_X.sh;" &
