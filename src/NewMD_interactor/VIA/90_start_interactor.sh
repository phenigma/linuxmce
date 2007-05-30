#!/bin/bash

if [[ -f /etc/network/interfaces.pbackup ]]; then
	cp /etc/network/interfaces{,.pbackup}
fi

echo "#loopback
auto lo
iface lo inet loopback

#network card
auto eth0
iface eth0 inet dhcp
" >/etc/network/interfaces

/etc/init.d/networking restart

MyIP=$(/sbin/ifconfig eth0 | awk 'NR==2 { print substr($2, index($2, ":") + 1) }')
MyMAC=$(/sbin/ifconfig eth0 | awk 'NR==1 { print $5 }')
Gateway=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $2 }')

Architecture="686"

echo "IP: $MyIP; MAC: $MyMAC; Gateway: $Gateway; Architecture: $Architecture"

if [[ -z "$MyIP" || -z "$MyMAC" ]]; then
	echo "Failed to get address from DHCP"
	exit 1
fi

/usr/pluto/install/via-interactor "$Gateway" "$MyIP" "$MyMAC" "$Architecture"

# get configs from server
