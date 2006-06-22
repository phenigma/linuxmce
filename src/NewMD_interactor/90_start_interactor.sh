#!/bin/sh

MyIP=$(/sbin/ifconfig eth0 | awk 'NR==2 { print substr($2, index($2, ":") + 1) }')
MyMAC=$(/sbin/ifconfig eth0 | awk 'NR==1 { print $5 }')
Gateway=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $2 }')

echo "IP: $MyIP; MAC: $MyMAC; Gateway: $Gateway"

/sbin/interactor "$Gateway" "$MyIP" "$MyMAC"
