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
#cp "$File" "$File.%(date +%F-%T)"
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

	DNSservers=$(grep nameserver /etc/resolv.conf | grep -v '#' | sed 's/nameserver//g; s/ *//g' | tr '\n' ' ')
fi

if [ -n "$IntIf" ]; then
	IfConf="auto $IntIf
	iface $IntIf inet static
		address $IntIP
		netmask $IntNetmask"
	echo "$IfConf" >>"$File"
fi

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

AskPort80="
Right now your Pluto Core will only allow you to access from within the local
network in your home. (Technical: All incoming ports are blocked).  

Do you want to access the Pluto web sites from a web browser outside the home
(ie open port 80)?  Note that if you answer 'yes' we recommend you get an SSL
certificate to encrypt the connection, otherwise it would be possible for
someone to get your username and password and gain access to the system. If in
doubt, just answer 'no'.

Allow web access? [y/N] "

AskPort22="
Do you want to allow incoming SSH connections by opening port 22?  This is a
secure way of accessing the system, but is only for techies and Linux users.

Allow SSH connections? [y/N]?"

#clear

#echo -n "$AskPort80"
#read Answer

#if [ "$Answer" == "y" -o "$Answer" == "Y" ]; then
#	echo "Storing your option to open port 80 (allowing web access)"
#	Q="INSERT INTO Firewall(Protocol,SourcePort,RuleType) VALUES('tcp',80,'core_input')"
#	RunSQL "$Q"
#fi

#echo -n "$AskPort22"
#read Answer

#if [ "$Answer" == "y" -o "$Answer" == "Y" ]; then
#	echo "Storing your option to open port 22 (allowing SSH access)"
#	Q="INSERT INTO Firewall(Protocol,SourcePort,RuleType) VALUES('tcp',22,'core_input')"
#	RunSQL "$Q"
#fi
