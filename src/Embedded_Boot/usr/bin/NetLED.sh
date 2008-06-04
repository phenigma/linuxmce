#!/bin/sh

trap 'Cleanup' EXIT
trap 'exit' INT

Cleanup()
{
	if [[ -n "$PID_NoNet" ]]; then
		kill "$PID_NoNet"
	fi
	echo 1 >/proc/diag/led/power
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
	for key in $Keys; do
		eval "server=\$(echo \$Srv_$key)"
		IP=$(/usr/bin/NetworkTest $server 80 2>/dev/null)
		Reachable=$?
		eval "IP_$key=$IP"
		eval "Reach_$key=$Reachable"
	done

	LastIP=
	for key in $Keys; do
		eval "IP=\$(echo \$IP_$key)"
		eval "Reach=\$(echo \$Reach_$key)"
		if [[ "$Reach" != 0 ]]; then
			NoInternet
			sleep 5
			continue 2
		fi
		if [[ "$IP" == "$LastIP" ]]; then
			NoInternet
			sleep 5
			continue 2
		fi
		LastIP="$IP"
	done
	break
done
