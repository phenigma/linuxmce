#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

FILLDB=/usr/bin/mythfilldatabase

# If the user installed a their own mythfilldatabase, adjust..
if test -f /usr/local/bin/mythfilldatabase ; then
	FILLDB=/usr/bin/mythfilldatabase
fi

# Run mythfilldatabase on a single core if possible
if test x"" != x'which taskset' ; then
	FILLDB="taskset -c 0 "$FILLDB
fi

HAS_IGNORE_BACKEND=`$FILLDB --help | grep ignore-backend`
if test x"" != x"$HAS_IGNORE_BACKEND" ; then
	FILLDB=$FILLDB" --ignore-backend"
fi

# Download the minimum data needed to get the channel lists
# $FILLDB --only-update-channels
# ^^^^ Not needed since we download today's data below...

# Download today's scheduling data, so that we can tune LiveTV immediately
$FILLDB --refresh-today --max-days 1

# Download the channel icons
$FILLDB --import-icon-map /home/mythtv/master_iconmap.xml --update-icon-map

# lock schema, this makes sure that we don't run this while the DB schema is
# being upgraded by another mythtv process.
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | mysql mythconverg
# Myth puts the icons in the current user's home directory.  We want /home/mythtv
echo "UPDATE channel SET icon=REPLACE(icon,'/root/.mythtv','/home/mythtv')" | mysql mythconverg
# Move the icons..
mkdir -p /home/mythtv/channels
if test x`echo/root/.mythtv/channels/*`x != x""x ; then
	mv -f /root/.mythtv/channels/* /home/mythtv/channels
	chmod a+r /home/mythtv/channels/*
fi

# Send a sync command to fix up the directories
/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 824

R=$(echo "SELECT * FROM videosource LIMIT 1" | mysql mythconverg)
if [[ -n "$R" ]]; then
	# Notify all the orbiters that myth is ready to be used
	/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 12 1 406 9 \
		"MythTV is ready.  However it may take several more minutes before the guide data is fully populated." 102 300
	/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 910 9 \
		"Finished retrieving channels"
fi

/usr/pluto/bin/Restart_Backend_With_SchemaLock.sh

# Now that we have channels and icons, go ahead and fill the database.  If the user interrupts this now it's ok
# because it will grab partial data and continue the next time
$FILLDB

# tell the backend to reschedule
mythbackend --resched > /dev/null &
