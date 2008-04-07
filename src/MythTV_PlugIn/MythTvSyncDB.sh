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

# if our options are all in one long quoted string... fix this.
if [ x"$2" = x"" ] ; then
    if [ x$MYTHTV_SYNC_DB_LOOP != x"1" ] ; then
        export MYTHTV_SYNC_DB_LOOP=1
        $0 $1
        exit
    fi
fi
export MYTHTV_SYNC_DB_LOOP=0

MYSQLPIPE="mysql --skip-column-names -B -h $DBHostName  -P $DBPort -u $DBUserName -p$DBPassword $DBName"
NAMES=$(echo "SELECT hostname FROM settings WHERE hostname IS NOT NULL GROUP BY hostname;" | $MYSQLPIPE)

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

if [ x"$1" = x"master" ] ; then
	IPADDR="$2"
	if [ x"" = x"$IPADDR" ] ; then
		echo "Must set IP Address of MythTV Master server."
		exit 1
	fi

	# Do NOT remove this user job, it is required for MythTV recordings
	# to show up in LMCE's recording list with metadata.
	UpdateMythSettingAllHosts "JobAllowUserJob1" "1"
	UpdateMythSetting "AutoRunUserJob1"  "1" "NULL"
	UpdateMythSetting "UserJob1"         "/usr/pluto/bin/SaveMythRecording.sh %CHANID% %STARTTIME% %DIR% %FILE%" "NULL"
	UpdateMythSetting "UserJobDesc1"     "Save the recorded show into Pluto\'s database" "NULL"

	# This is only to default to English, if you set a different
	# set of languages within MythTV these will not be inserted.
	InsertMythSettingAllHosts "Language"                   "EN"
	InsertMythSettingAllHosts "ISO639Language0"            "eng"
	InsertMythSettingAllHosts "ISO639Language1"            "eng"

	# Changing any of these are know to cause problems within MythTV
	# LMCE will still function, but MythTV will not function optimally.
	UpdateMythSettingAllHosts "PreferredMPEG2Decoder"      "ffmpeg"
	UpdateMythSettingAllHosts "UseChromaKeyOSD"            "0"
	UpdateMythSettingAllHosts "UseOpenGLVSync"             "0"

	# This can be safely changed, but beware that Xine can not take
	# advantage of this flagging and it consumes considerable CPU.
	UpdateMythSettingAllHosts "AutoCommercialFlag"         "0"
	UpdateMythSettingAllHosts "AutoCommflagWhileRecording" "0"

	# Do NOT change the network control setup, the MythTV_PlugIn will get confused.
	UpdateMythSettingAllHosts "NetworkControlEnabled"      "1"
	UpdateMythSettingAllHosts "NetworkControlPort"         "10001"

	# This channel changing option can be safely changed
	UpdateMythSettingAllHosts "SelectChangesChannel"       "1"

	# These playback options can be safely changed
	UpdateMythSettingAllHosts "VertScanPercentage"         "2"
	UpdateMythSettingAllHosts "HorizScanPercentage"        "1"
	UpdateMythSettingAllHosts "Deinterlace"                "1"
	UpdateMythSettingAllHosts "DeinterlaceFilter"          "bobdeint"

	# Changing these will cause problems with fast forward in the Orbiter UI
	UpdateMythSettingAllHosts "FFRewSpeed0"                "1"
	UpdateMythSettingAllHosts "FFRewSpeed1"                "2"
	UpdateMythSettingAllHosts "FFRewSpeed2"                "4"
	UpdateMythSettingAllHosts "FFRewSpeed3"                "8"
	UpdateMythSettingAllHosts "FFRewSpeed4"                "16"

	# LMCE has it's own volume controls and the internal volume controls can interfere.
	# If you change this make sure that you configure the internal mixer to control the
	# PCM volume and not the master volume control.
	UpdateMythSettingAllHosts "MythControlsVolume" "0"

	# Don't disable mythfilldatabase even with EIT scheduling data.
	# mythfilldatabase also performs some housecleaning tasks.
	UpdateMythSetting "MythFillEnabled"     "1"     "NULL"
	UpdateMythSetting "MythFillPeriod"      "1"     "NULL"
	UpdateMythSetting "MythFillMinHour"     "0"     "NULL"
	UpdateMythSetting "MythFillMaxHour"     "19"    "NULL"
	UpdateMythSetting "MythFillDatabasePath" "/usr/pluto/bin/MythTvDailyFillDB.sh" "NULL"
	UpdateMythSetting "MythFillDatabaseLog" "/var/log/mythtv/mythfilldatabase.log" "NULL"
	UpdateMythSetting "MythFillSuggestedRunTime" "1970-01-01T00:00:00" "NULL"

	# Don't change this unless you KNOW what you are doing
	UpdateMythSetting "MasterServerIP"      $IPADDR "NULL"
	UpdateMythSetting "MasterServerPort"    "6543"  "NULL"
elif [ x"$1" = x"slave" ] ; then
	IPADDR="$2"
	NAME="$3"
	if [ x"" = x"$IPADDR" ] ; then
		echo "Must set IP Address of MythTV server."
		exit 1
	fi
	if [ x"" = x"$NAME" ] ; then
		echo "Must set name of MythTV server."
		exit 2
	fi

	# Don't change this unless you KNOW what you are doing
	UpdateMythSetting "BackendServerIP"     $IPADDR $NAME
	UpdateMythSetting "MasterServerPort"    "6543"  $NAME
	UpdateMythSetting "BackendServerPort"   "6543"  $NAME
	UpdateMythSetting "BackendServerStatus" "6544"  $NAME
fi
