#!/bin/bash

# Takes as a parameter the name of a program, and it will forcibly kill it, first by sending
# the processes a kill, and then a kill -9 if they don't die

process_name="$1"
running_pids=$(pidof $process_name)

if [[ "$running_pids" == "" ]] ;then 
	echo "No program with that name running ."
	exit 0
fi

echo "Killing : $running_pids"
kill $running_pids
start_time=$(date +%s)
echo -n "Waiting to die "
while [[ $(( start_time + 5 )) -gt $( date +%s ) ]] ;do
	
	left_running=""
	for pid in $running_pids ;do
		if [[ -d /proc/$pid ]] ;then
			left_running="$left_running $pid"
		fi
	done

	if [[ "$left_running" != "" ]] ;then
		echo -n "."
		running_pids="$left_running"
	else
		echo
		exit 0
	fi
done
echo

echo "Force Killing: $running_pids"
kill -9 $running_pids
