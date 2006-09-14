#!/bin/bash

. /usr/pluto/bin/Utils.sh

ReloadDevicesOnThisMachine
pidOfX=$(ps ax|grep 'X :0 -ignoreABI -ac -allowMouseOpenFail vt7'|egrep -v 'grep|SCREEN'|awk '{print $1}')
kill $pidOfX
sleep 5
/usr/pluto/bin/Start_X.sh
