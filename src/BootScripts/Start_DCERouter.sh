#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh

trap 'Unlock "DCER" "Start_DCE_Router"' EXIT
Lock "DCER" "Start_DCE_Router"

cd /usr/pluto/bin
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting DCERouter"

LogFile="/var/log/pluto/DCERouter.log";

module=DCERouter
device_name="$module"

screen -d -m -S DCERouter /usr/pluto/bin/Spawn_DCERouter.sh
# wait for DCERouter to start listening on socket
while ! nc -z localhost 3450; do
	sleep 0.1
done

if [[ "$(pidof discovery.py)" == "" ]] ;then
	screen -d -m -S discovery /usr/pluto/bin/discovery.py
fi
