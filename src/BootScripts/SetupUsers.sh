#!/bin/bash

cd /usr/pluto/bin
./SetupUsers_Homes.sh
if [ -x /usr/pluto/bin/generate_users_voicemenu.pl ]
then
	./generate_users_voicemenu.pl
fi

