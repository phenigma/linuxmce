#!/bin/sh
# /usr/pluto/bin/MythTvSyncDB.sh
# Copyright 2008 Daniel Kristjansson
# Licensed under the GPL, version 2; or, at your option, a later version.

DBHostName="localhost"
DBUserName="mythtv"
DBName="mythconverg"
DBPassword="mythtv"
DBPort="3306"

if [ -f /etc/mythtv/mysql.txt ]; then
	. /etc/mythtv/mysql.txt
fi

MYSQLPIPE="mysql --skip-column-names -B -h $DBHostName  -P $DBPort -u $DBUserName -p$DBPassword $DBName"
MYSQLPIPE_WITH_COL="mysql -B -h $DBHostName  -P $DBPort -u $DBUserName -p$DBPassword $DBName"

function UpdateMythSetting {
	SETTING_VALUE="$1"
	SETTING_DATA="$2"
	SETTING_HOST="$3"

	if [ x"$SETTING_HOST" = x"NULL" ] ; then
		echo "DELETE FROM settings WHERE value = '${SETTING_VALUE}' AND hostname IS NULL;" | $MYSQLPIPE
		echo "INSERT INTO settings(value,data) VALUES ('$SETTING_VALUE','$SETTING_DATA');" | $MYSQLPIPE
	else
		echo "DELETE FROM settings WHERE value = '${SETTING_VALUE}' AND hostname = '${SETTING_HOST}';" | $MYSQLPIPE
		echo "INSERT INTO settings(value,data,hostname) VALUES ('$SETTING_VALUE', '$SETTING_DATA', '$SETTING_HOST');" | $MYSQLPIPE
	fi
}

function InsertMythSetting {
	SETTING_VALUE="$1"
	SETTING_DATA="$2"
	SETTING_HOST="$3"

	if [ x"$SETTING_HOST" = x"NULL" ] ; then
		COUNT=$(echo "SELECT count(1) FROM settings WHERE value = '${SETTING_VALUE}' AND hostname IS NULL;" | $MYSQLPIPE)
	else
		COUNT=$(echo "SELECT count(1) FROM settings WHERE value = '${SETTING_VALUE}' AND hostname = '${SETTING_HOST}';" | $MYSQLPIPE)
	fi

	if [ x"$COUNT" == x"0" ] ; then
		UpdateMythSetting "$SETTING_VALUE" "$SETTING_DATA" "$SETTING_HOST"
	fi
}

function UpdateMythSettingAllHosts {
	SETTING_VALUE="$1"
	SETTING_DATA="$2"

	for NAME in $NAMES ; do
		UpdateMythSetting "$SETTING_VALUE" "$SETTING_DATA" "$NAME"
	done
}

function InsertMythSettingAllHosts {
	SETTING_VALUE="$1"
	SETTING_DATA="$2"

	for NAME in $NAMES ; do
		InsertMythSetting "$SETTING_VALUE" "$SETTING_DATA" "$NAME"
	done	
}
