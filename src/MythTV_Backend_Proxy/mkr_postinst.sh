
eval `cat /etc/mythtv/mysql.txt | grep -v "^#" | grep -v "^$"`;

mysql_command="mysql -s -B -u $DBUserName -h $DBHostName -p$DBPassword $DBName";

query="select count(*) from settings where hostname='`hostname`' AND value LIKE 'Backend%'";

function addEntries
{
	if [ $# != 2 ]; then 
		echo "This function needs 2 parameters";
		return;
	fi;

	echo "insert into settings(value, data, hostname) values('$1', '$2', '`hostname`')" | $mysql_command;
}

isSetup=`echo "$query" | $mysql_command`;

if [ "$isSetup" != 0  ]; then
	echo "Removing old mythtv configuration for this machine";
	echo "delete from settings where hostname='`hostname`' AND value LIkE 'Backend%'" | $mysql_command;
	echo "delete from settings where hostname='`hostname`' AND value LIkE 'RecordFilePrefix'" | $mysql_command;
fi;

echo "Setting up mythtv configuration for this machine";
routerIPAddress=`gethostip dcerouter | cut -f 2 -d ' '`;
addEntries BackendServerIP $routerIPAddress;
addEntries BackendServerPort 6543;
addEntries BackendServerStatus 6544;
addEntries RecordFilePrefix /var/lib/mythtv/;

