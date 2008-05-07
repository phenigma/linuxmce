#!/bin/bash

echo $0 start `date`

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/MythTvCommon.sh

# Append all params..
FILLDB=$FILLDB" $@"

# Before we lock, make sure MythTVInitialFillDB.sh was not run recently.
# This is so that we don't start this until the user has finished
# configuring MythTV in the configuration Wizard. By checking this
# before we lock we make sure we're not running this script when
# MythTVInitialFillDB.sh needs to be rerun.
# Even if we are MythTvInitFillDB.sh can kill us, but this prevents
# a lot of false starts from happening.
WaitUntilMythTvInitialRunXMinutesOld 2

# Lock the MythFillDatabase lock to another script from running mythfilldatabase
echo "$0 waiting for MythFillDatabase lock"
WaitLock "MythFillDatabase" "MythTvDailyFillDB.sh" nolog
echo "$0 got MythFillDatabase lock"

# lock schema, this makes sure that we don't run this while the DB schema is
# being upgraded by another mythtv process.
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | $MYSQLPIPE

CHAN_COUNT_BFR=$(echo "SELECT count(1) FROM channel;" | $MYSQLPIPE)

# Throttle the load so as to allow LiveTV to still work...
UpdateMythSetting "DDSlowDownFactor" "5" "NULL"

# Download scheduling data
$FILLDB

CHAN_COUNT_AFT=$(echo "SELECT count(1) FROM channel;" | $MYSQLPIPE)

if [ x"$CHAN_COUNT_BFR" == x"$CHAN_COUNT_AFT" ] ; then
	# Tell the user it's now safe to use MythTV
	R=$(echo "SELECT * FROM videosource LIMIT 1" | $MYSQLPIPE_WITH_COL)
	if [[ -n "$R" ]]; then
		# Notify all the orbiters that myth is ready to be used
		/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 910 9 \
			"Finished retrieving scheduling data"
	fi
fi

# Unlock the MythFillDatabase lock
Unlock "MythFillDatabase" "MythTvDailyFillDB.sh" nolog

# if we ran without a mythbackend connection, tell the backend to reschedule..
if test x"" != x"$HAS_IGNORE_BACKEND" ; then
	$BACKEND --resched > /dev/null &
fi

# if channels were added we need to redownload channel icons
if [ x"$CHAN_COUNT_BFR" != x"$CHAN_COUNT_AFT" ] ; then
	DownloadChannelIcons
fi

echo $0 end `date`
