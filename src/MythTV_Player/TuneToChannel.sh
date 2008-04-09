#!/bin/bash

. /etc/mythtv/mysql.txt

SQL_COMMAND="mysql -N -h $DBHostName -p$DBPassword -u $DBUserName $DBName";

echo "Got: 0:$0 1:$1 2:$2 3:$3"

QUERY="\
SELECT xmltvid \
FROM channel \
    WHERE channum='$3' AND sourceid='$2'";

XMLTV_ID=$(echo "$QUERY" | $SQL_COMMAND);

echo "$QUERY"
echo "/usr/pluto/bin/MessageSend dcerouter 0 $1 1 187 68 $3 39 $XMLTV_ID"
/usr/pluto/bin/MessageSend dcerouter 0 $1 1 187 68 $3 39 "$XMLTV_ID"
