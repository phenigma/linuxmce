#!/bin/bash


MYTH_SETUP_PIDS=`pidof mythtv-setup`;

if [ "$MYTH_SETUP_PIDS" != "" ]; then 
	kill -9 $MYTH_SETUP_PIDS;
fi;

echo -e "\n\n" | mythtv-setup 

/etc/init.d/mythtv-backend force-reload

PID=`pidof mmythfilldatabase`;

if [ "$PID" != "" ] ; then 
	kill -9 $PID;
fi;

/etc/cron.daily/mythtv-backend 2>/var/log/pluto/myth-filldatabase.log &

# sleep 1;

# /usr/pluto/bin/ratpoison -c "select mythtv-setup";
