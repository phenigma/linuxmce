#!/bin/bash

# define error codes

ERR_NOT_ENOUGH_PARAMS=100;
ERR_SOFTWARE_NOT_INSTALLED=101;
ERR_COULD_NOT_TEST=102;

ERR_SUCCESS=0;

BINARY_NAME=dvdcss_server;

[ $# != 2 ] && exit $ERR_NOT_ENOUGH_PARAMS;

PORT=$1
FILE=$2;

SOFTWARE_BINARY=`which $BINARY_NAME`;

[ "$SOFTWARE_BINARY" == "" ] && exit $ERR_SOFTWARE_NOT_INSTALLED;

echo -e "\n*********************\nMounting \"$FILE\" on port $PORT at time: `date`\n" >> /var/log/pluto/dvd_mounts.log;

OLD_PIDS=`ps aux | grep $BINARY_NAME | cut -c 10-15`;

if [ "$OLD_PIDS" != "" ]; then 
	echo "Killing the previous instance(s): $OLD_PIDS"  >> /var/log/pluto/dvd_mounts.log;
	kill -15 $OLD_PIDS || true;
fi

# Limit the number of file descriptors to a sane value. Not all the 
# application open sockets and never close them.
ulimit -n 128
$SOFTWARE_BINARY "$FILE" $PORT >> /var/log/pluto/dvd_mounts.log &

TRIES=5;
SLEEP=1;
while [ $TRIES != 0 ]; do
	echo $TRIEs;
	if nc -z localhost $PORT; then 
		exit $ERR_SUCCESS;
	else
		sleep $SLEEP;
		TRIES=$[$TRIES - 1];
	fi;
done;

exit $ERR_COULD_NOT_TEST;
