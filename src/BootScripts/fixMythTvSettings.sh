#!/bin/bash

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

hasSchema=`echo show tables | $mysql_command | grep settings | wc -l`;
if [ "$hasSchema" != "1" ]; then
	echo "There is no schema in the mythconverg database. Exiting";
	exit 1;
fi

isSetup=`echo "$query" | $mysql_command`;

if [ "$isSetup" != 0  ]; then
        echo "Removing old mythtv configuration for this machine";
        echo "delete from settings where hostname='`hostname`' AND value LIkE 'Backend%'" | $mysql_command;
        echo "delete from settings where value LIkE 'MasterServer%'" | $mysql_command;
        echo "delete from settings where hostname='`hostname`' AND value LIkE 'RecordFilePrefix'" | $mysql_command;
fi;

echo "Setting up mythtv configuration for this machine";
hostname=`hostname`;
hostip=`gethostip $hostname | cut -f 2 -d ' '`;
routerip=`gethostip dcerouter | cut -f 2 -d ' '`;

addEntries BackendServerIP $hostip $hostname;
addEntries BackendServerPort 6143 $hostname;
#
# The is for Myth 0.17 * They changed the names
#
# addEntries BackendStatusPort 6143 $hostname;
addEntries BackendServerStatus 6544 $hostname;
addEntries RecordFilePrefix /var/lib/mythtv/ $hostname;
# addEntries LiveBufferDir /var/cache/mythtv/ $hostname;

addEntries MasterServerIP       $routerip;
addEntries MasterServerPort     6543;

