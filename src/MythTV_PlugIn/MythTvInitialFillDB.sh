#!/bin/bash

echo $0 start `date`

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/MythTvCommon.sh

# This sets a timestamp which prevents MythTvDailyFillDB.sh from taking the lock..
# We do this a few times in case this takes a while..
SetMythTvInitialFillDBRun

# Kill any full DB fills, these will be restarted later on by this script.
PIDTOKILL=`pidof -x MythTvDailyFillDB.sh`
if [ x"$PIDTOKILL" != x"" ] ; then
    kill $PIDTOKILL
fi

# Lock the MythFillDatabase lock to another script from running mythfilldatabase
echo "$0 waiting for MythFillDatabase lock"
WaitLock "MythFillDatabase" "MythTvInitialFillDB.sh" nolog
echo "$0 got MythFillDatabase lock"

SetMythTvInitialFillDBRun

# lock schema, this makes sure that we don't run this while the DB schema is
# being upgraded by another mythtv process.
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | $MYSQLPIPE

# Make sure mythfilldatabase log is owned by mythtv user
touch $FILLDB_LOG ; chown mythtv:mythtv $FILLDB_LOG

SetMythTvInitialFillDBRun

# Do this at full speed.. 
UpdateMythSetting "DDSlowDownFactor" "0" "NULL"

# Download today's scheduling data, so that we can tune LiveTV immediately
$FILLDB --refresh-today --max-days 1 >> $FILLDB_LOG

SetMythTvInitialFillDBRun

# Download the channel icons
DownloadChannelIcons >> $FILLDB_LOG

SetMythTvInitialFillDBRun

# Send a sync command to fix up the directories
/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 824

# Tell MythTV to run a full DB sync as soon as possible
UpdateMythSetting "MythFillSuggestedRunTime" "1970-01-01T00:00:00" "NULL"
UpdateMythSetting "MythFillMinHour"          "0"                   "NULL"
UpdateMythSetting "MythFillMaxHour"          "23"                  "NULL"
echo "UPDATE housekeeping SET lastrun = '1970-01-01 00:00:00' WHERE tag = 'MythFillDB';" | $MYSQLPIPE

# Restart all the backends..
/usr/pluto/bin/Restart_Backend_With_SchemaLock.sh

# Tell the user it's now safe to use MythTV
R=$(echo "SELECT * FROM videosource LIMIT 1" | $MYSQLPIPE_WITH_COL)
if [[ -n "$R" ]]; then
	echo Notify all the orbiters that myth is ready to be used
	/usr/pluto/bin/MessageSend dcerouter 0 -305 1 741 159 53 9 \
		"MythTV is ready. However, it may take some time before the guide data is fully populated."
	/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 910 9 \
		"Finished retrieving channels"
fi

SetMythTvInitialFillDBRun

# Unlock the MythFillDatabase lock
Unlock "MythFillDatabase" "MythTvInitialFillDB.sh" nolog

# if we ran without a mythbackend connection, tell the backend to reschedule..
if test x"" != x"$HAS_IGNORE_BACKEND" ; then
	$BACKEND --resched > /dev/null &
fi

echo $0 end `date`
