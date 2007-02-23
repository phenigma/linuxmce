#!/bin/bash

. /tmp/mce_wizard_data.sh

function Setup_Network_Intefaces {
	if [[ $c_netExtUseDhcp  == "1" ]] ;then
		echo "iface $c_netExtName inet dhcp" > /etc/network/interfaces
		dhclient $c_netExtName
	else
		echo > /etc/network/interfaces
		echo "auto lo" >> /etc/network/interfaces
		echo "iface lo inet loopback" >> /etc/network/interfaces
		echo "" >> /etc/network/interfaces
		echo "auto $c_netExtName" >> /etc/network/interfaces
		echo "iface $c_netExtName inet static" >> /etc/network/interfaces
		echo "address $c_netExtIP" >> /etc/network/interfaces
		echo "netmask $c_netExtMask" >> /etc/network/interfaces
		echo "gateway $c_netExtGateway" >> /etc/network/interfaces

		echo > /etc/resolv.conf
		echo "nameserver $c_netExtDNS1" >> /etc/resolv.conf

		ifconfig $c_netIntName $c_netExtIP netmask $c_netExtMask up
		route add default gw $c_netExtGateway
	fi

}


Setup_Network_Intefaces

echo > /tmp/mce-wget-progress
mkdir -p /var/linuxmce/
while read line; do
	text=$(echo $line | sed -u 's/  */ /g' | sed -u 's/^ //g' | sed -u 's/%//g' | awk '{print $7 "|" $1 "|" $8 " " $9}')
	if echo "$text" | egrep -q '^[0-9]{1,3}\|[0-9]*' ;then
		echo "$text" >> /tmp/mce-wget-progress
	fi
done < <(wget -c -O /var/linuxmce/linux-mce.iso --progress=dot  ftp://ftp.proxad.net/pub/Distributions_Linux/GeeXboX/releases/1.0/geexbox-1.0-en.i386.iso 2>&1 1>/dev/null)


