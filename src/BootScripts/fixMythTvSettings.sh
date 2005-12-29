#!/bin/bash

if [ ! -e /etc/mythtv/mysql.txt ]; then
	echo "Mythtv doesn't seem to be installed here";
	exit 0;
fi;

chown -R mythtv.mythtv /etc/mythtv/mysql.txt
chown root.root /etc/mythtv
chmod 751 /etc/mythtv
chmod 760 /etc/mythtv/mysql.txt

eval `cat /etc/mythtv/mysql.txt | grep -v "^#" | grep -v "^$"`;

mysql_command="mysql -s -B -u $DBUserName -h $DBHostName -p$DBPassword $DBName";
query="select count(*) from settings where hostname='`hostname`' AND value LIKE 'Backend%'";

function addEntries
{
        if [ "$3" = "" ]; then
                echo "insert into settings(value, data) values('$1', '$2')" | $mysql_command;
        else
		echo "delete from settings where hostname='$3' AND value LIKE '$1'" | $mysql_command;
                echo "insert into settings(value, data, hostname) values('$1', '$2', '$3')" | $mysql_command;
        fi;
}

hasSchema=`echo show tables | $mysql_command | grep settings | wc -l`;
if [ "$hasSchema" != "1" ]; then
	echo "There is no schema in the mythconverg database. Exiting";
	exit 1;
fi

echo "Setting up mythtv configuration for this machine";
hostname=`hostname`;
hostip=`gethostip $hostname | cut -f 2 -d ' '`;
routerip=`gethostip dcerouter | cut -f 2 -d ' '`;

isSetup=`echo "$query" | $mysql_command`;

if [ "$isSetup" != 0  ]; then
        echo "Removing old mythtv configuration for this machine";
		echo "delete from settings where value LIkE 'MasterServer%'" | $mysql_command;
fi;

addEntries MasterServerIP       $routerip;

if ["$hostip" != "$routerip"]; then
	addEntries MasterServerPort     6543;
else
	addEntries MasterServerPort     6143;
fi;

addEntries BackendServerIP 		$hostip $hostname;
addEntries BackendServerPort 	6143 	$hostname;

addEntries RecordFilePrefix 	/home/mythtv/shows/$hostip $hostname;
addEntries LiveBufferDir 		/home/mythtv/cache/$hostip $hostname;
addEntries VertScanPercentage	2	$hostname;
addEntries HorizScanPercentage	1	$hostname;
addEntries NetworkControlPort	10001	$hostname;
addEntries UseXVMC 1	$hostname;
addEntries Deinterlace 1	$hostname;
addEntries DeinterlaceFilter bobdeint	$hostname;

echo -n Creating and/or setting the cache and recorded folder paths ...
mkdir -p 		/home/mythtv/shows/$hostip
chown mythtv 	/home/mythtv/shows/$hostip
chmod 766 	 	/home/mythtv/shows/$hostip

mkdir -p 		/home/mythtv/cache/$hostip
chown mythtv 	/home/mythtv/cache/$hostip
chmod 766 	 	/home/mythtv/cache/$hostip

echo done.

/etc/init.d/mythtv-backend force-reload
