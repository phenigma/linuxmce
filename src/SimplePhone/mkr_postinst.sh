#!/bin/bash

echo "Syncing pluto 2 amp"
if [ -x /usr/pluto/bin/sync_pluto2amp.pl ]
then
	/usr/pluto/bin/sync_pluto2amp.pl
else
	if [ -n "`ls -la /etc/ssh/*.pub`" ]
	then
		/usr/pluto/bin/LaunchRemoteCmd.sh dcerouter "/usr/pluto/bin/sync_pluto2amp.pl"
	fi
fi

