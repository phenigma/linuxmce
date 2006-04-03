#!/bin/sh

msg="Please wait while MD filesystem is created. System will reboot when done"
fil="************************************************************************"

bright="[1m"
color="[1;33m"
normal="[0m"

MyIP=$(/sbin/ifconfig eth0 | awk 'NR==2 { print substr($2, index($2, ":")) }')
MyMAC=$(/sbin/ifconfig eth0 | awk 'NR==1 { print $5 }')
Gateway=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $2 }')

echo "IP: $MyIP; MAC: $MyMAC; Gateway: $Gateway"

echo "$bright**$fil**$normal"
echo "$bright* $color$msg$normal$bright *$normal"
echo "$bright**$fil**$normal"

/sbin/interactor "$Gateway" "$MyIP" "$MyMAC"
