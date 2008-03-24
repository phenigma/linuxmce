#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

eval `cat /etc/mythtv/mysql.txt | grep -v "^#" | grep -v "^$"`;

mysql_command="mysql -s -B -u $DBUserName -h $DBHostName -p$DBPassword $DBName";

# Lock the MythBackend lock to prevent backend restart by another script...
WaitLock "MythBackend" "Restart_Backend_With_SchemaLock.sh" nolog

# Make sure we're not in the middle of a schema upgrade -- myth doesn't check this
echo "LOCK TABLE schemalock WRITE; UNLOCK TABLES;" | $mysql_command

# Actually perform the restart..
invoke-rc.d mythtv-backend restart &> /dev/null

# Restart slave backends as well
SLAVES=`echo "SELECT hostname from capturecard;" | $mysql_command | grep -v dcerouter | grep -v NULL`
if [ x"$SLAVES" != x"" ] ; then
        for SLAVE in $SLAVES ; do
                ssh $SLAVE "invoke-rc.d mythtv-backend restart" &> /dev/null
        done
fi

# Unlock the MythBackend lock so auto restart will work elsewhere
Unlock "MythBackend" "Restart_Backend_With_SchemaLock.sh" nolog
