#!/bin/bash

. /usr/pluto/bin/LockUtils.sh

# Lock the MythBackend lock to prevent backend restarting
WaitLock "MythBackend" "launchMythSetup" nolog

MYTH_SETUP_PIDS=`pidof mythtv-setup`;

if [[ -n "$MYTH_SETUP_PIDS" ]]; then 
	killall mythtv-setup
fi;

/etc/init.d/mythtv-backend stop
PID=`pidof mythbackend`;

if [ "$PID" != "" ] ; then 
	kill -9 $PID;
fi;

echo -e "\n\n" | mythtv-setup 

/etc/init.d/mythtv-backend force-reload

PID=`pidof mythfilldatabase`;

if [ "$PID" != "" ] ; then 
	kill -9 $PID;
fi;

/etc/cron.daily/mythtv-backend 2>/var/log/pluto/myth-filldatabase.log &

# sleep 1;

# /usr/pluto/bin/ratpoison -c "select mythtv-setup";
Unlock "MythBackend" "launchMythSetup" nolog
