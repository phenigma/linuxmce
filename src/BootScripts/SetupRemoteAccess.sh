#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

if [ -n "$remote" ]; then
	# if user doesn't exist
		# add user "remote" w/ the password specified (var value)
	# if port forward is not active
		# create the ssh tunnel 22 w/ port=PK_Installation*2+10000
		# create the ssh tunnel 80 w/ port=PK_Installation*2+10001
	# if this script is not in a cron job
		# add it to be run every 10 mins
else
	# delete user "remote" if it exists
	# kill port forwards if they exist
	# remove script from cron job
fi
