#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

mkdir -p /var/log/pluto
echo "$(date) Spawn_Device $*" >> /var/log/pluto/Spawn_Device.log
# syntax: $0 <device_id> <ip_of_router> <cmd_line> 

. /usr/pluto/bin/pluto.func || exit

TYPE=spawning-device
device_id="$1"
ip_of_router="$2"
device_name="$device_id";
module="$device_name" #$(basename $0)
cmd_line="$3"

Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "device: $device_id ip: $ip_of_router cmd_line: $cmd_line"

log_file="/var/log/pluto/${device_id}_`basename $cmd_line`";
valgrind_log_file="/var/log/pluto/valgrind_${device_id}_`basename $cmd_line`";
new_log="$log_file.newlog"
real_log="$log_file.log"

[ -n "$VGcmd" ] && VGcmd="$VGcmd --log-file=$valgrind_log_file "

if [ -z "$cmd_line" ]; then
	Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "Empty command line"
	exit 1
fi

OK=0
if [ -x "./$cmd_line" ]; then
	Path="./"
	OK=1
elif [ -x "/usr/pluto/bin/$cmd_line" ]; then
	Path="/usr/pluto/bin/"
	cd $Path
	OK=1
elif [ -x "$cmd_line" ]; then
	Path=
	OK=1
else
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0 $module" "Can't execute $cmd_line"
	exit 1
fi

if [ -z "$display" ]; then
	display=":0"
fi

[ -z "$DISPLAY" ] && export DISPLAY=:0

Logging $TYPE $SEVERITY_STAGE "$module" "Entering $module"

echo "$$ Spawn_Device of $Path$cmd_line (by $0)" >>/var/log/pluto/running.pids

Tab=$(printf "\t") # to prevent any smart masses from converting this tab to 4 or 8 spaces
i=1
while [ "$i" -le 10 ]; do
	rm -f /var/log/pluto/spawned_devices_$device_id

	Logging $TYPE $SEVERITY_NORMAL "$module" "Appending log..."
	cat "$new_log" >> "$real_log"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i"
	echo $(date) Starting > "$new_log"

	Logging $TYPE $SEVERITY_NORMAL "$module" "Found $Path$cmd_line"
	if [ "${cmd_line/Spawn_Device}" == "$cmd_line" ] && [ "${Valgrind/$cmd_line}" != "$Valgrind" ]; then
#		(/usr/pluto/bin/Spawn_Wrapper.sh $(echo "$VGcmd")"$Path$cmd_line" -d "$device_id" -r "$ip_of_router"; echo $? >"/tmp/pluto_exitcode_$device_id") | tee "$new_log"
		/usr/pluto/bin/Spawn_Wrapper.sh $(echo "$VGcmd")"$Path$cmd_line" -d "$device_id" -r "$ip_of_router" > >(tee "$new_log")
	else
#		(/usr/pluto/bin/Spawn_Wrapper.sh "$Path$cmd_line" -d "$device_id" -r "$ip_of_router"; echo $? >"/tmp/pluto_exitcode_$device_id") | tee "$new_log"
		/usr/pluto/bin/Spawn_Wrapper.sh "$Path$cmd_line" -d "$device_id" -r "$ip_of_router" > >(tee "$new_log")
	fi

#	Ret="$(cat /tmp/pluto_exitcode_$device_id)"
	Ret="$?"
	rm -f /tmp/pluto_exitcode_$device_id
	
	if [ -f /var/log/pluto/spawned_devices_$device_id ]; then
		while read line; do
			screen -list | grep -F "$line" | cut -d. -f1 | cut -d"$Tab" -f2 | xargs kill -9
		done < /var/log/pluto/spawned_devices_$device_id
	fi
	screen -wipe &>/dev/null
	
	if [ "$Ret" -eq 3 ]; then
		# Abort
		Logging $TYPE $SEVERITY_WARNING "$module" "Shutting down... $i $device_name"
		echo $(date) Shutdown >> "$new_log"
		exit
	elif [ "$Ret" -eq 2 ]; then
		Logging $TYPE $SEVERITY_WARNING "$module" "Device requests restart... $i $device_name"
		echo $(date) Shutdown >> "$new_log"
		sleep 10
	else
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... $i $device_name"
		echo $(date) died >> "$new_log"
		echo $(date) $device_name died >> /var/log/pluto/deaths
		echo $(date) died >> /var/log/pluto/died_${device_id}_$device_name
		sleep 5
		i=$((i+1))
	fi
	echo out
done
#if [ "$i" -eq 10 ]; then
#	Logging $TYPE $SEVERITY_CRITICAL $module "Aborting restart of device..."
#	printf "01\tAborting restart of device $device_name\n" >> /var/log/pluto/PlutoServer.log
#	wget -O /dev/null "www.1control.com/critical_error.php?installation=$INSTALLATION&Message=Device_${device_id}_failed"
#fi
