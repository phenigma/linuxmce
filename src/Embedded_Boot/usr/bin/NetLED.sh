#!/bin/sh

if [[ -f /tmp/netled.pid ]]; then
	exit
fi

echo $$ >/tmp/netled.pid

trap 'Cleanup' EXIT
trap 'exit' INT

Cleanup()
{
	if [[ -n "$PID_NoNet" ]]; then
		kill "$PID_NoNet"
	fi
	echo 1 >/proc/diag/led/power
	rm -f /tmp/netled.pid
}

LED_NoNet()
{
	while :; do
		for i in 0 1 0 1; do
			echo "$i" >/proc/diag/led/power
			Sleep 100
		done
		Sleep 500
	done
}

NoInternet()
{
	if [[ -z "$PID_NoNet" ]]; then
		LED_NoNet &
		PID_NoNet=$!
	fi
}

Keys="google youtube yahoo plutohome"

Srv_google=google.com
Srv_youtube=youtube.com
Srv_yahoo=yahoo.com
Srv_plutohome=plutohome.com

PID_NoNet=

while :; do
	Reach=0
	for key in $Keys; do
		eval "server=\$(echo \$Srv_$key)"
		IP=$(/usr/bin/NetworkTest $server 80 2>/dev/null)
		Reachable=$?
		eval "IP_$key=$IP"
		if [[ "$Reachable" -eq 0 ]]; then
			Reach=$(expr $Reach + 1)
		fi
	done

	if [[ "$Reach" -eq 0 ]]; then
		NoInternet
		sleep 5
		continue
	fi

	LastIP=
	for key in $Keys; do
		eval "IP=\$(echo \$IP_$key)"
		if [[ "$IP" == "$LastIP" ]]; then
			NoInternet
			sleep 5
			continue 2
		fi
		LastIP="$IP"
	done
	break
done

if [[ ! -f /etc/pluto/fresh_install ]]; then
	exit
fi

IPtype=routable

if ifconfig | grep -q br-lan:0; then
	WANip=$(ifconfig br-lan:0 | grep 'inet addr:' | sed 's/.*inet addr:\([0-9.]*\).*/\1/g')
else
	WANip=$(ifconfig eth0.1 | grep 'inet addr:' | sed 's/.*inet addr:\([0-9.]*\).*/\1/g')
fi

d1=$(echo "$WANip" | cut -d. -f1)
d2=$(echo "$WANip" | cut -d. -f2)

if [[ "$d1" == 10 ]]; then
	IPtype=local
elif [[ "$d1" == 192 ]] && [[ "$d2" == 168 ]]; then
	IPtype=local
elif [[ "$d1" == 172 ]] && [[ "$d2" -ge 16 ]] && [[ "$d2" -le 31 ]]; then
	IPtype=local
fi

Cfg=$(uci show dhcp|grep interface=lan|cut -d. -f2)
CfgMasq=$(uci show dhcp|grep =dnsmasq|cut -d. -f2|cut -d= -f1)
uci set dhcp.$Cfg.force=1
uci set dhcp.$Cfg.ignore=0
case "$IPtype" in
	local)
		uci set dhcp.$Cfg.dynamicdhcp=0
		uci set dhcp.$CfgMasq.authoritative=0
		uci set network.eth0.vlan0="0 1 2 3 4 5*"
		uci del network.eth0.vlan1
		uci set network.wan.ifname=br-lan:0
		ifconfig eth0.1 down
		;;
	routable)
		uci set dhcp.$Cfg.dynamicdhcp=1
		uci set dhcp.$CfgMasq.authoritative=1
		
		Model=$(cat /proc/diag/model)
		case "$Model" in
			"ASUS WL-500g Premium")
				uci set network.eth0.vlan0="1 2 3 4 5*"
				uci set network.eth0.vlan1="0 5"
				;;
			"ASUS WL-500g Premium V2")
				uci set network.eth0.vlan0="0 1 2 3 5*"
				uci set network.eth0.vlan1="4 5"
				;;
		esac

		uci set network.wan.ifname=eth0.1
		;;
esac
uci commit

/etc/init.d/firewall restart
/etc/init.d/dnsmasq restart
/etc/init.d/network restart
