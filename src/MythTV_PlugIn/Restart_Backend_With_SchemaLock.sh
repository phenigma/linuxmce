#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

eval `cat /etc/mythtv/mysql.txt | grep -v "^#" | grep -v "^$"`;

mysql_command="mysql -s -B -u $DBUserName -h $DBHostName -p$DBPassword $DBName";


echo "LOCK TABLE schemalock WRITE;" | $mysql_command # Be sure we're not in the middle of a schema upgrade -- myth doesn't check this
invoke-rc.d mythtv-backend restart

