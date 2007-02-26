#!/bin/bash

if [[ -n "$(pgrep Dhcpd-Plugin.sh)" ]]; then
	exit 0 # already running
fi

date -R
echo Launched the script
/usr/bin/tail -F /var/log/syslog | grep --line-buffered 'dhcpd: ' | bash -x /usr/pluto/bin/Dhcpd-Plugin-Parser.sh

echo Exiting
