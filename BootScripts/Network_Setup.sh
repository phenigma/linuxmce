#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh

if [ "$NetIfConf" -eq 0 ]; then
	echo "Populating network settings from current system config"
	NCards=$(lspci | grep Ethernet | wc -l)
	NetSettings=$(ParseInterfaces)
	ExtData=$(echo "$NetSettings" | head -1)
	ExtractData "$ExtData"
	if [ -z "$DHCPsetting" ]; then
		echo "No DHCP Setting found. Not storing internal interface data"
		if [ "$ExtIP" == "dhcp" ]; then
			NetIfConf="$ExtIf,dhcp|"
		else
			NetIfConf="$ExtIf,$ExtIP,$ExtNetmask,$Gateway,$DNS|"
		fi
	else
		echo "Using DHCP Setting to set up internal interface data: '$DHCPsetting'"
		echo "Found $NCards network cards"
		IntNetmask="255.255.255.0"
		IntIP="$(echo "$DHCPsetting" | cut -d. -f-3).1"
		if [ "$NCards" -eq 1 ]; then
			IntIf="eth0:0"
		elif [ "$NCards" -eq 2 ]; then
			[ "$ExtIf" == "eth0" ] && IntIf="eth1" || IntIf="eth0"
		fi
		if [ "$ExtIP" == "dhcp" ]; then
			NetIfConf="$ExtIf,dhcp|$IntIf,$IntIP,$IntNetmask"
		else
			NetIfConf="$ExtIf,$ExtIP,$ExtNetmask,$Gateway,$DNS|$IntIf,$IntIP,$IntNetmask"
		fi
	fi
	Q="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES('$PK_Device',32,'$NetIfConf')"
	RunSQL "$Q"
fi

echo "Writing network configuration with one in database"

/etc/init.d/networking stop

File=/etc/network/interfaces
IfConf="auto lo
iface lo inet loopback
"
echo "$IfConf" >"$File"

[ "$ExtIP" == "dhcp" ] && Setting="dhcp" || Setting="static"
if [ "$Setting" == "static" ]; then
	IfConf="auto $ExtIf
	iface $ExtIf inet static
		address $ExtIP
		netmask $ExtNetmask
		gateway $Gateway"
	echo "$IfConf" >>"$File"

	DNSservers=$(echo "$DNS,"; echo "$NetSettings" | tail -1)
	DNSservers=$(echo "$DNSservers" | tr ',' '\n' | sort -u | tr '\n' ' ')
	: >/etc/resolv.conf
	for i in $DNSservers; do
		echo "nameserver $i" >>/etc/resolv.conf
	done
else
	IfConf="auto $ExtIf
	iface $ExtIf inet dhcp"
	echo "$IfConf" >>"$File"
fi

IfConf="auto $IntIf
iface $IntIf inet static
	address $IntIP
	netmask $IntNetmask"
echo "$IfConf" >>"$File"

if [ -n "$DHCPcard" ]; then
	echo "INTERFACES=\"$DHCPcard\"" >/etc/default/dhcp3-server
elif [ -n "${IntIf##*:*}" ]; then
	echo "INTERFACES=\"$IntIf\"" >/etc/default/dhcp3-server
fi

/etc/init.d/networking start

Fwd="forwarders {"
for i in $DNSservers; do
	Fwd=$(printf "%s" "$Fwd~!$i;")
done
Fwd=$(printf "%s" "$Fwd~};~")
echo "$Fwd" | tr '~!' '\n\t' >/etc/bind/named.conf.forwarders

awk 'BEGIN { Replace = 0 }
/\/\/ forwarders/ {
	Replace = 3;
	print("include \"/etc/bind/named.conf.forwarders\";\n");
}
Replace == 0 { print }
Replace != 0 { Replace-- }
' /etc/bind/named.conf.options >/etc/bind/named.conf.options.$$
mv /etc/bind/named.conf.options.$$ /etc/bind/named.conf.options
rndc reload

