#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/MythTvCommon.sh

# Lock the MythFillDatabase lock to another script from running mythfilldatabase
echo "MythTvInitialFillDB.sh waiting for MythFillDatabase lock"
WaitLock "MythFillDatabase" "MythTvInitialFillDB.sh" nolog
echo "MythTvInitialFillDB.sh got MythFillDatabase lock"

# lock schema, this makes sure that we don't run this while the DB schema is
# being upgraded by another mythtv process.
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | $MYSQLPIPE

# Make sure mythfilldatabase log is owned by mythtv user
touch $FILLDB_LOG ; chown mythtv:mythtv $FILLDB_LOG

# Download today's scheduling data, so that we can tune LiveTV immediately
$FILLDB --refresh-today --max-days 1 >> $FILLDB_LOG

# Download the channel icons
DownloadChannelIcons >> $FILLDB_LOG

# Send a sync command to fix up the directories
/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 824

# Tell MythTV to run a full DB sync as soon as possible
UpdateMythSetting "MythFillSuggestedRunTime" "1970-01-01T00:00:00" "NULL"

# Restart all the backends..
/usr/pluto/bin/Restart_Backend_With_SchemaLock.sh

# Tell the user it's now safe to use MythTV
R=$(echo "SELECT * FROM videosource LIMIT 1" | $MYSQL_WITH_COL)
if [[ -n "$R" ]]; then
	# Notify all the orbiters that myth is ready to be used
	/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 12 1 406 9 \
		"MythTV is ready.  However it may take several more minutes before the guide data is fully populated." 102 300
	/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 910 9 \
		"Finished retrieving channels"
fi

# Unlock the MythFillDatabase lock
Unlock "MythFillDatabase" "MythTvInitialFillDB.sh" nolog

# if we ran without a mythbackend connection, tell the backend to reschedule..
if test x"" != x"$HAS_IGNORE_BACKEND" ; then
	$BACKEND --resched > /dev/null &
fi
