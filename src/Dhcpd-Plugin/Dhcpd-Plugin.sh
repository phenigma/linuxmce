#!/bin/bash

date -R
echo Launched the script
/usr/bin/tail -F /var/log/syslog | grep --line-buffered 'dhcpd: ' | bash -x /usr/pluto/bin/Dhcpd-Plugin-Parser.sh

echo Exiting
