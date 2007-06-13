#!/bin/bash

echo "$(date -R) SetupUsers" >> /var/log/pluto/SetupUsers.log
cd /usr/pluto/bin
bash -x ./SetupUsers_Homes.sh >> /var/log/pluto/SetupUsers.log
if [ -x /usr/pluto/bin/generate_users_voicemenu.pl ]
then
	./generate_users_voicemenu.pl
fi

