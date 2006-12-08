#!/bin/bash

#mds=$(cat /proc/partitions | awk '/(md)/ { print $4 }')

#for md in $mds; do
#	new_mds="$new_mds/dev/$md "
#done

#PID=$(cat /tmp/raid_pid)
#kill -9 "$PID"
md=$1
mdadm --monitor --delay=300 --daemonise --program=/usr/pluto/bin/monitoring_RAID.sh "$md"

#echo $PID > /tmp/raid_pid

