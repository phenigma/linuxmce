#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

cd /usr/pluto/bin
echo "Starting DCERouter"

screen -d -m -S DCERouter bash -c "(echo \$(date) Starting; /usr/pluto/bin/DCERouter -h localhost) | tee DCERouter.log"
timeout=60
waited=0
while ! nc -z localhost 3450 &>/dev/null && [ "$waited" -lt "$timeout" ]; do
	sleep 1
	waited=$((waited + 1))
done
if [ "$waited" -eq "$timeout" ]; then
	echo "--> DCERouter failed to start"
	exit 1
else
	echo "--> DCERouter started ok"
fi
