#!/bin/bash

F()
{
	echo "$2" | tr '~' ' ' | cut -d" " -f"$1"
}

for proc in $(cat /var/log/pluto/running.pids | tr '\n ' ' ~'); do
	PID=$(F 1 $proc)
	cmdline=$(F 2- $proc)
	echo "Stopping $cmdline ($PID)"
	kill -INT $PID
done

rm /var/log/pluto/running.pids
