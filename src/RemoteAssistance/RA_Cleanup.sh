#!/bin/bash

# NOTE: This runs on the website to cleanup the allocated ports

PassFile=/etc/pluto/RA_cleanup.password

if [[ ! -r "$PassFile" ]]; then
	echo "Can't read password file"
	exit 1
fi

GracePeriod=604800 #seconds, 1week

Q="
	UPDATE RemoteAssistance
	SET
		EK_Installation = NULL,
		DateAllocated = NULL,
		LastActive = NULL
	WHERE DATE_SUB(NOW(), INTERVAL $GracePeriod SECOND) > LastActive
"

echo "$Q;" | mysql -uroot -p"$(<"$PassFile")" pluto_website
