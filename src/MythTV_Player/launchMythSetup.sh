#!/bin/bash


MYTH_SETUP_PIDS=`pidof mythtv-setup`;

if [ "$MYTH_SETUP_PIDS" != "" ]; then 
	kill -9 $MYTH_SETUP_PIDS;
fi;

echo -e "\n\n" | mythtv-setup 

/etc/init.d/mythtv-backend force-reload &

