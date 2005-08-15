#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

eval `cat /etc/mythtv/mysql.txt | grep -v "^#" | grep -v "^$"`;

mysql_command="mysql -s -B -u $DBUserName -h $DBHostName -p$DBPassword $DBName";

query="select count(*) from settings where hostname='`hostname`' AND value LIKE 'Backend%'";

function addEntries
{
	if [ "$3" = "" ]; then
		echo "insert into settings(value, data) values('$1', '$2')" | $mysql_command;
	else
		echo "insert into settings(value, data, hostname) values('$1', '$2', '$3')" | $mysql_command;
	fi;
}

# make the proper ownership's because the backend can't read it otherwise
chown mythtv /etc/mythtv/mysql.txt

#Force the backend to make the database structure
/etc/init.d/mythtv-backend force-reload || /bin/true

echo "Waiting 3 seconds so that myth backed is able to actually create the schema"

FOUND=0;
TRIES=20;
SLEEP=0.5;
while [ $TRIES != 0 ]; do
    RESULT=`echo show tables | $mysql_command | grep settings | wc -l`;
    if [ "$RESULT" == "1" ]; then
        TRIES=0;
        FOUND=1;
    else
        sleep $SLEEP;
        TRIES=$[$TRIES - 1];
    fi;
done;

if [ "$FOUND" == "0" ]; then 
	echo "********";
	echo "There is an error in the mythtv backend setup. It looks like the database schema wasn't created properly";
	echo "********";
	exit 0;
fi

isSetup=`echo "$query" | $mysql_command`;

if [ "$isSetup" != 0  ]; then
	echo "Removing old mythtv configuration for this machine";
	echo "delete from settings where hostname='`hostname`' AND value LIkE 'BackendServer%'" | $mysql_command;
	echo "delete from settings where value LIkE 'MasterServer%'" | $mysql_command;
	echo "delete from settings where hostname='`hostname`' AND value LIkE 'RecordFilePrefix'" | $mysql_command;
fi;

echo "Setting up mythtv configuration for this machine";
hostname=`hostname`;
hostip=`gethostip $hostname | cut -f 2 -d ' '`;
routerip=`gethostip dcerouter | cut -f 2 -d ' '`;

addEntries BackendServerIP $hostip $hostname;
addEntries BackendServerPort 6143 $hostname;
addEntries BackendServerStatus 6544 $hostname;
addEntries RecordFilePrefix /var/lib/mythtv/ $hostname;

addEntries MasterServerIP	$routerip;
addEntries MasterServerPort	6543;

PID=`pidof /usr/bin/mythbackend`;
if [ "$PID" != "" ]; then
	# It seems that in certain circumstances the backed refuses to notice the -9 signal
	kill -9 $PID;
	/etc/init.d/mythtv-backend start || /bin/true
fi

#Create a shortcut to MythTV Setup
#Find the media director -- it could be a child if this is a hybrid
Q="SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=8 AND (PK_Device=$PK_Device OR FK_Device_ControlledVia=$PK_Device) LIMIT 1"
PK_Device_MD="$(RunSQL "$Q")"

#Do we have a quick launch already?
Q="SELECT PK_Device_QuickStart FROM Device_QuickStart where FK_Device=$PK_Device_MD AND Description='MythTV Setup'"
PK_Device_QuickStart="$(RunSQL "$Q")"

if [ "$PK_Device_QuickStart" = "" ]; then
	Q="INSERT INTO Device_QuickStart(FK_Device,Description,SortOrder,\`Binary\`,Arguments) VALUES($PK_Device_MD,'MythTV Setup',1,'/bin/bash','-c\\t/usr/pluto/bin/launchMythSetup.sh')"
	RunSQL "$Q"
fi
