#!/bin/dash

MyIP=$(/sbin/ifconfig eth0 | awk 'NR==2 { print substr($2, index($2, ":") + 1) }')
MyMAC=$(/sbin/ifconfig eth0 | awk 'NR==1 { print $5 }')
Gateway=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $2 }')

Architecture="686"
is2ndCpu=$(awk '/processor.*: 1/' /proc/cpuinfo)
if [ -n "$is2ndCpu"  ] ;then
	Architecture="686-smp"
fi

echo "IP: $MyIP; MAC: $MyMAC; Gateway: $Gateway; Architecture: $Architecture"


/sbin/interactor "$Gateway" "$MyIP" "$MyMAC" "$Architecture"
