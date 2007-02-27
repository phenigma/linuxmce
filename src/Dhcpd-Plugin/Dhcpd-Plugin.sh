#!/bin/bash

PID=$(pgrep Dhcpd-Plugin.sh)
PID="${PID//$$}"
PID="${PID// }"
if [[ -n "$PID" ]]; then
	exit 0 # already running
fi

date -R
echo Launched the script
/usr/bin/tail -F /var/log/syslog | grep --line-buffered 'dhcpd: ' | bash -x /usr/pluto/bin/Dhcpd-Plugin-Parser.sh

echo Exiting
