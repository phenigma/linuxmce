#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting ratpoison"

export DISPLAY=:0
screen -d -m -S RatPoison /usr/pluto/bin/ratpoison
sleep 1

PID=$(ps ax | grep /usr/pluto/bin/ratpoison | egrep -v 'SCREEN|grep' | awk '{ print $1 }')
echo "$PID ratpoison (by $0)" >>/var/log/pluto/running.pids
