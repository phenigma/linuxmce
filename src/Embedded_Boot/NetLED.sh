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

PID_NoNet=
while :; do
	for server in google.com youtube.com yahoo.com plutohome.com; do
		if wget -sqO/dev/null http://"$server"/ 2>/dev/null; then
			break 2
		fi
	done
	if [[ -z "$PID_NoNet" ]]; then
		LED_NoNet &
		PID_NoNet=$!
	fi
done

