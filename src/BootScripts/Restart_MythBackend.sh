#!/bin/bash

# XXX: This script restarts the mythtv backend if it should be running but isn't

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Network_Parameters.sh

while :; do
	WaitLock "MythBackend" "MythBackend_Restart" nolog
	IsRunning=$(pgrep mythbackend)
	if [[ -z "$IsRunning" && -x /etc/init.d/mythtv-backend ]]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "MythBackend_Restart" "MythBackend not found running; restarting it"
		/etc/init.d/mythtv-backend restart
	fi
	Unlock "MythBackend" "MythBackend_Restart" nolog
	sleep 5
done
