#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

cd /usr/pluto/bin
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting DCERouter"

screen -d -m -S DCERouter bash -c "(echo \$(date) Starting; /usr/pluto/bin/DCERouter -h localhost) | tee DCERouter.log"
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
fi
