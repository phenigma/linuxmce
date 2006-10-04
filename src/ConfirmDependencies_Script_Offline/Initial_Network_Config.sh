#!/bin/bash

NetworkString=$1
NetworkStringDigits=$2

IntIf=
IntIP=
IntNetmask=
ExtIf=
ExtIP=
ExtNetmask=
Gateway=
DNS=
NetIfConf=0
NCards=$(ifconfig -a | grep -c "encap:Ethernet")


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
/iface/ && $3 == "inet" {
        if (ParsingInterface == 1)
        {
                Display(ParsingInterface, interface, address, netmask, gateway, dns);
        }
        interface = $2;
        if (interface == "lo")
        {
                ParsingInterface = 0;
                next;
        }
        setting = $4;
        if (setting == "static")
        {
                ParsingInterface = 1;
                dns="";
        }
        else if (setting == "dhcp")
        {
                ParsingInterface = 1;
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


## ExtIf, ExtIp, ExtNetmask, Gateway, DNS
ExtractedString=$(ParseInterfaces | head -1 )
ExtIf=$( echo $ExtractedString | cut -d',' -f1 )
ExtIP=$( echo $ExtractedString | cut -d',' -f2 )
if [ "$ExtIP" != "DHCP" -a "$ExtIP" != "dhcp" ]; then
	ExtNetmask=$(echo $ExtractedString | cut -d',' -f3)
	Gateway=$(echo $ExtractedString | cut -d',' -f4)
	DNS=$(grep nameserver /etc/resolv.conf | grep -v '#' | sed 's/nameserver//g; s/ *//g' | tr '\n' ',' | sed 's/,$//')
fi


## IntIP + IntNetmask
IntIP="$NetworkString"
IntNetmask=""
for i in `seq 1 $NetworkStringDigits` ;do
	if [[ "$IntNetmask" == "" ]] ;then
		IntNetmask="255"
	else
		IntNetmask="${IntNetmask}.255"
	fi
done

for i in `seq $NetworkStringDigits 3` ;do
	if [[ $i == "3" ]] ;then
		IntIP="${IntIP}.1"
	else
		IntIP="${IntIP}.0"
	fi

	IntNetmask="${IntNetmask}.0"
done

if [[ "$NCards" -eq 1 ]]; then
	ntIf="eth0:0"
else
	[[ "$ExtIf" == "eth0" ]] && IntIf="eth1" || IntIf="eth0"
fi
        
if [[ "$ExtIP" == "dhcp" ]]; then
	NetIfConf="$ExtIf,dhcp|$IntIf,$IntIP,$IntNetmask"
else
	NetIfConf="$ExtIf,$ExtIP,$ExtNetmask,$Gateway,$DNS|$IntIf,$IntIP,$IntNetmask"
fi

echo $NetIfConf
