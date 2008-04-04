#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

FILLDB=/usr/bin/mythfilldatabase

# If the user installed a their own mythfilldatabase, adjust..
if test -f /usr/local/bin/mythfilldatabase ; then
	FILLDB=/usr/local/bin/mythfilldatabase
fi

# Run mythfilldatabase on a single core if possible
if test x"" != x'which taskset' ; then
	FILLDB="taskset -c 0 "$FILLDB
fi

HAS_IGNORE_BACKEND=`$FILLDB --help | grep ignore-backend`
if test x"" != x"$HAS_IGNORE_BACKEND" ; then
	FILLDB=$FILLDB" --ignore-backend"
fi

# Append all params..
FILLDB=$FILLDB" $@"

# Lock the MythFillDatabase lock to another script from running mythfilldatabase
echo "MythTvDailyFillDB.sh waiting for MythFillDatabase lock"
WaitLock "MythFillDatabase" "MythTvDailyFillDB.sh" nolog
echo "MythTvDailyFillDB.sh got MythFillDatabase lock"

# lock schema, this makes sure that we don't run this while the DB schema is
# being upgraded by another mythtv process.
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | mysql mythconverg

# Download today's scheduling data, so that we can tune LiveTV immediately
$FILLDB

# Tell the user it's now safe to use MythTV
R=$(echo "SELECT * FROM videosource LIMIT 1" | mysql mythconverg)
if [[ -n "$R" ]]; then
	# Notify all the orbiters that myth is ready to be used
	/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 910 9 \
		"Finished retrieving scheduling data"
fi

# Unlock the MythFillDatabase lock
Unlock "MythFillDatabase" "MythTvDailyFillDB.sh" nolog

# if we ran without a mythbackend connection, tell the backend to reschedule..
if test x"" != x"$HAS_IGNORE_BACKEND" ; then
	mythbackend --resched > /dev/null &
fi
