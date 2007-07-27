#!/bin/bash

while read line; do
	line="${line%%:*}"
	line="${line// }"
	if [[ "$line" != eth* ]]; then
		continue
	fi
	Interfaces=("${Interfaces[@]}" "$line")
done </proc/net/dev

echo -n "Does the network use DHCP? [Y/n]: "
read DHCP

i=1
echo "Which network interface to set up?"
for Interface in "${Interfaces[@]}"; do
	echo "$i. $Interface"
	((i++))
done
read Number

Interface="${Interfaces[$((Number-1))]}"
echo "Chosen interface: $Interface"

if [[ -z "$DHCP" || "$DHCP" == y || "$DHCP" == Y ]]; then
	dhclient $Interface
else
	killall dhclient &>/dev/null

	echo -n "IP: "; read IP
	echo -n "Netmask: "; read Netmask
	echo -n "Gateway: "; read Gateway
	echo -n "DNS: "; read DNS

	ifconfig $Interface $IP netmask $Netmask up
	echo "nameserver $DNS" >/etc/resolv.conf
	route del default 2>/dev/null || :
	route add default gw $Gateway
fi
