#!/bin/bash

RunSQL()
{
	local Q
	Q="$*"
	[ -z "$Q" ] || echo "$Q;" | mysql pluto_main | tail +2 | tr '\n\t ' ' ,~'
}

Field()
{
	local Row FieldNumber
	FieldNumber="$1"; shift
	Row="$*"
	echo "$Row" | cut -d, -f"$FieldNumber" | tr '~' ' '
}

InRange()
{
	local Static Dynamic StaticDigit1 StaticDigit2 DynamicDigit1 DynamicDigit2
	local Digit DHCPsetting

	Digit="$1"
	DHCPsetting="$2"

	Static=$(Field 1 "$DHCPsetting")
	Dynamic=$(Field 2 "$DHCPsetting")

	StaticDigit1=$(echo "$Static" | cut -d- -f1 | cut -d. -f4)
	StaticDigit2=$(echo "$Static" | cut -d- -f2 | cut -d. -f4)
	DynamicDigit1=$(echo "$Dynamic" | cut -d- -f1 | cut -d. -f4)
	DynamicDigit2=$(echo "$Dynamic" | cut -d- -f2 | cut -d. -f4)

	[ "$Digit" -ge "$StaticDigit1" -a "$Digit" -le "$StaticDigit2" ]
	return "$?"
}

ExtractData()
{
	local R IntPart ExtPart
	R="$*"
	if [ -n "$R" ]; then
		# format:
		# <Ext If>,<Ext IP>,<Ext Netmask>,<Gateway>,<Ext DNS CSV>|<Int If>,<Int IP>,<Int Netmask>
		# <Ext If>,DHCP|<Int If>,<Int IP>,<Int Netmask>
		ExtPart=$(echo "$R" | cut -d'|' -f1)
		IntPart=$(echo "$R" | cut -d'|' -sf2)

		if [ -n "$IntPart" ]; then
			IntIf=$(Field 1 "$IntPart")
			IntIP=$(Field 2 "$IntPart")
			IntNetmask=$(Field 3 "$IntPart")
		fi

		if [ -n "$ExtPart" ]; then
			ExtIf=$(Field 1 "$ExtPart")
			ExtIP=$(Field 2 "$ExtPart")
			if [ "$ExtIP" != "DHCP" -a "$ExtIP" != "dhcp" ]; then
				ExtNetmask=$(Field 3 "$ExtPart")
				Gateway=$(Field 4 "$ExtPart")
				DNS=$(Field 5- "$ExtPart")
			fi
		fi
		NetIfConf=1
	fi
}

ParseInterfaces()
{
	local Script File

	File="/etc/network/interfaces"

	Script='
function Display(ParsingInterface, interface, address, netmask, gateway, dns)
{
	if (ParsingInterface == 0)
		return;
	if (substr(dns, length(dns), 1) == ",")
		dns = substr(dns, 1, length(dns) - 1);
	if (address == "dhcp")
		print (interface, address);
	else
		print (interface, address, netmask, gateway, dns);
}

BEGIN { ParsingInterface = 0; OFS = ","; }
/#/ {
	$0 = substr($0, 1, match($0, /#/) - 1);
}
/auto (.*)/ {
	Display(ParsingInterface, interface, address, netmask, gateway, dns);
	ParsingInterface = 0;
	if ($2 == "lo") next;
	interface = $2;
}
/iface/ && $2 == interface && $3 == "inet" {
	setting = $4;
	if (setting == "static")
	{
		ParsingInterface = 1;
		dns="";
	}
	else if (setting == "dhcp")
	{
		address = "dhcp";
	}
}
ParsingInterface == 1 {
	if ($1 == "address")
		address = $2;
	else if ($1 == "netmask")
		netmask = $2;
	else if ($1 == "gateway")
		gateway = $2;
	else if ($1 == "dns-nameservers")
		dns = dns $2 ",";
}
END { Display(ParsingInterface, interface, address, netmask, gateway, dns); }'
	awk "$Script" "$File"

	grep nameserver /etc/resolv.conf | grep -v '#' | sed 's/nameserver//g; s/ *//g' | tr '\n' ',' | sed 's/,$//'
}

IntIf=
IntIP=
IntNetmask=
ExtIf=
ExtIP=
ExtNetmask=
Gateway=
DNS=
NetIfConf=0

Q="SELECT IK_DeviceData
FROM Device_DeviceData
WHERE FK_DeviceData=32"
R=$(RunSQL "$Q")

ExtractData "$R"

Q="SELECT IK_DeviceData
FROM Device_DeviceData
JOIN Device ON PK_Device=FK_Device
JOIN DeviceTemplate ON PK_DeviceTemplate=FK_DeviceTemplate
WHERE FK_DeviceTemplate=7 AND FK_DeviceData=28"
DHCPsetting=$(RunSQL "$Q")
if [ "${DHCPsetting#eth*,}" != "$DHCPsetting" ]; then
	DHCPcard=${DHCPsetting%%,*}
fi
