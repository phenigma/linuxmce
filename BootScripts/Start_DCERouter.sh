#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

cd /usr/pluto/bin
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting DCERouter"

log_file="/var/log/pluto/DCERouter";
new_log="$log_file.newlog"
real_log="$log_file.log"

Logging $TYPE $SEVERITY_NORMAL "$module" "Appending log..."
cat "$new_log" >> "$real_log"
Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i"
echo $(date) Starting > "$new_log"

screen -d -m -S DCERouter bash -c "(echo \$(date) Starting; $VGcmd/usr/pluto/bin/DCERouter -h localhost) | tee $new_log"
timeout=60
waited=0
while ! nc -z localhost 3450 &>/dev/null && [ "$waited" -lt "$timeout" ]; do
	sleep 1
	waited=$((waited + 1))
done
if [ "$waited" -eq "$timeout" ]; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "--> DCERouter failed to start"
	exit 1
else
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "DCERouter started ok"
	PID=$(ps ax | grep /usr/pluto/bin/DCERouter | egrep -v 'SCREEN|grep|bash' | awk '{ print $1 }')
	echo "$PID DCERouter (by $0)" >>/var/log/pluto/running.pids
fi
