#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

FILLDB=/usr/bin/mythfilldatabase
FILLDB_LOG=/var/log/mythtv/mythfilldatabase.log

DBHostName="localhost"
DBUserName="mythtv"
DBName="mythconverg"
DBPassword="mythtv"
DBPort="3306"

if [ -f /etc/mythtv/mysql.txt ]; then
	. /etc/mythtv/mysql.txt
fi

mysql_command="mysql --skip-column-names -B -h $DBHostName -P $DBPort -u $DBUserName -p$DBPassword $DBName"
mysql_command2="mysql -B -h $DBHostName -P $DBPort -u $DBUserName -p$DBPassword $DBName"

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

# Lock the MythFillDatabase lock to another script from running mythfilldatabase
echo "MythTvInitialFillDB.sh waiting for MythFillDatabase lock"
WaitLock "MythFillDatabase" "MythTvInitialFillDB.sh" nolog
echo "MythTvInitialFillDB.sh got MythFillDatabase lock"

# lock schema, this makes sure that we don't run this while the DB schema is
# being upgraded by another mythtv process.
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | $mysql_command

# Make sure mythfilldatabase log is owned by mythtv user
touch $FILLDB_LOG ; chown mythtv:mythtv $FILLDB_LOG

# Download today's scheduling data, so that we can tune LiveTV immediately
$FILLDB --refresh-today --max-days 1 >> $FILLDB_LOG

# Download the channel icons
$FILLDB --import-icon-map /home/mythtv/master_iconmap.xml --update-icon-map >> $FILLDB_LOG

# Myth puts the icons in the current user's home directory.  We want /home/mythtv
echo "UPDATE channel SET icon=REPLACE(icon,'/root/.mythtv','/home/mythtv')" | $mysql_command
# Move the icons..
mkdir -p /home/mythtv/channels
if test x`echo/root/.mythtv/channels/*`x != x""x ; then
	mv -f /root/.mythtv/channels/* /home/mythtv/channels
	chmod a+r /home/mythtv/channels/*
fi

# Send a sync command to fix up the directories
/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 824

# Restart all the backends..
/usr/pluto/bin/Restart_Backend_With_SchemaLock.sh

# Tell the user it's now safe to use MythTV
R=$(echo "SELECT * FROM videosource LIMIT 1" | $mysql_command2)
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
	mythbackend --resched > /dev/null &
fi
