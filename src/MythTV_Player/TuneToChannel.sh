#!/bin/bash

. /etc/mythtv/mysql.txt

SQL_COMMAND="mysql -N -h $DBHostName -p$DBPassword -u $DBUserName $DBName";


QUERY="\
SELECT input.cardinputid \
FROM cardinput input \
	JOIN capturecard card ON card.cardid=input.cardid \
WHERE externalcommand='$0 $1' and hostname='`hostname`'";

CONNECTION_ID=$(echo "$QUERY" | $SQL_COMMAND);
echo "Found this connection id: $CONNECTION_ID and channel number: $2 "; 


/usr/pluto/bin/MessageSend dcerouter 0 $1 1 187 68 $2

