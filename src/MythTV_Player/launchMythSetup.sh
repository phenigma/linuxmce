#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Config_Ops.sh

export DISPLAY=:$Display

MythPass=$(cat /etc/mythtv/mysql.txt |grep ^DBPassword|cut -d= -f2)
MysqlCommand="mysql -D mythconverg -h $MySqlHost -u mythtv -p$MythPass";

SETUP=/usr/bin/mythtv-setup

# Use mythtv-setup.real if it exists..
if test -f /usr/bin/mythtv-setup.real ; then
	SETUP=/usr/bin/mythtv-setup.real
fi

# If the user installed their own mythtv-setup, adjust..
if test -f /usr/local/bin/mythtv-setup ; then
	SETUP=/usr/local/bin/mythtv-setup
fi

# Run mythtv-setup on a single core if possible
if test x"" != x'which taskset' ; then
        SETUP="taskset -c 0 "$SETUP
fi

# Lock the MythBackend lock to prevent backend restarting
WaitLock "MythBackend" "launchMythSetup" nolog

MYTH_SETUP_PIDS=`pidof mythtv-setup`;
if [[ -n "$MYTH_SETUP_PIDS" ]]; then 
	killall mythtv-setup
fi;

MYTH_SETUP_PIDS=`pidof mythtv-setup.real`;
if [[ -n "$MYTH_SETUP_PIDS" ]]; then 
	killall mythtv-setup.real
fi;

# Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | $MysqlCommand
invoke-rc.d mythtv-backend stop
PID=`pidof mythbackend`;

if [ "$PID" != "" ] ; then 
	kill -9 $PID;
fi;

$SETUP

invoke-rc.d mythtv-backend force-reload

PID=`pidof mythfilldatabase`;

if [ "$PID" != "" ] ; then 
	kill -9 $PID;
fi;

/etc/cron.daily/mythtv-backend 2>/var/log/pluto/myth-filldatabase.log &

# sleep 1;

# /usr/pluto/bin/ratpoison -c "select mythtv-setup";
Unlock "MythBackend" "launchMythSetup" nolog
