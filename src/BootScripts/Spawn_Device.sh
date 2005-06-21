#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/LockUtils.sh

MAX_RESPAWN_COUNT=50

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

AlreadyRunning="/usr/pluto/locks/pluto_spawned_local_devices.txt"
WaitLock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Already Running list: $(cat $AlreadyRunning | tr '\n' ',')"
if grep -q "^$device_id$" "$AlreadyRunning" 2>/dev/null; then
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Device $device_id was marked as 'running'. Not starting"
	echo "$(date) Already running: $device_id" >>/var/log/pluto/Spawn_Device.log
	Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Exiting"
	exit
fi
echo "$device_id" >>"$AlreadyRunning"
echo "$(date) About to be run: $device_id" >>/var/log/pluto/Spawn_Device.log
Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log

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
ReloadWatcher=0
while [ "$i" -le "$MAX_RESPAWN_COUNT" ]; do
	rm -f /var/log/pluto/spawned_devices_$device_id

	Logging $TYPE $SEVERITY_NORMAL "$module" "Appending log..."
	cat "$new_log" >> "$real_log"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i"
	echo $(date) Starting > "$new_log"

	if [[ "$ReloadWatcher" -eq 1 ]]; then
		/usr/pluto/bin/Start_LocalDevices.sh
	fi

	ReloadLock=/usr/pluto/locks/reload_watcher
	if [[ "${cmd_line//App*Server}" != "$cmd_line" && ! -f "$ReloadLock" ]]; then
		echo "Device: $device_id" >"$ReloadLock"
		ReloadWatcher=1
	fi
	Logging $TYPE $SEVERITY_NORMAL "$module" "Found $Path$cmd_line"
	if [ "${cmd_line/Spawn_Device}" == "$cmd_line" ] && [ "${Valgrind/$cmd_line}" != "$Valgrind" ]; then
#		(/usr/pluto/bin/Spawn_Wrapper.sh $(echo "$VGcmd")"$Path$cmd_line" -d "$device_id" -r "$ip_of_router"; echo $? >"/tmp/pluto_exitcode_$device_id") | tee "$new_log"
		/usr/pluto/bin/Spawn_Wrapper.sh $(echo "$VGcmd")"$Path$cmd_line" -d "$device_id" -r "$ip_of_router" &> >(tee "$new_log")
	else
#		(/usr/pluto/bin/Spawn_Wrapper.sh "$Path$cmd_line" -d "$device_id" -r "$ip_of_router"; echo $? >"/tmp/pluto_exitcode_$device_id") | tee "$new_log"
		/usr/pluto/bin/Spawn_Wrapper.sh "$Path$cmd_line" -d "$device_id" -r "$ip_of_router" &> >(tee "$new_log")
	fi

#	Ret="$(cat /tmp/pluto_exitcode_$device_id)"
	Ret="$?"
	rm -f /tmp/pluto_exitcode_$device_id
	
#	if [ -f /var/log/pluto/spawned_devices_$device_id ]; then
#		while read line; do
#			screen -list | grep -F "$line" | cut -d. -f1 | cut -d"$Tab" -f2 | xargs kill -9
#		done < /var/log/pluto/spawned_devices_$device_id
#	fi
#	screen -wipe &>/dev/null
	
	echo "Return code: $Ret" >>"$new_log"
	if [ "$Ret" -eq 3 ]; then
		# Abort
		Logging $TYPE $SEVERITY_WARNING "$module" "Shutting down... $i $device_name"
		echo $(date) Shutdown >> "$new_log"
		exit
	elif [ "$Ret" -eq 2 ]; then
		Logging $TYPE $SEVERITY_WARNING "$module" "Device requests restart... $i $device_name"
		echo $(date) Restart >> "$new_log"
		sleep 10
		#/usr/pluto/bin/Start_LocalDevices.sh
	else
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... $i $device_name"
		echo $(date) died >> "$new_log"
		echo $(date) $device_name died >> /var/log/pluto/deaths
		echo $(date) died >> /var/log/pluto/died_${device_id}_$device_name

		count=0
		echo /usr/pluto/coredump/core_${cmd_line##*/}_* | tr ' ' '\n' | while read line; do
			((count++))
			if [[ "$count" -gt 3 ]]; then
				rm -f $line
			fi
		done
		if [[ "$count" -gt 3 ]]; then
			Logging $TYPE $SEVERITY_WARNING "$module" "Found $count cores. Deleted all but latest 3. $i $device_name"
		fi
		sleep 20
		#/usr/pluto/bin/Start_LocalDevices.sh
		i=$((i+1))
	fi
	echo out
done
#if [ "$i" -eq 10 ]; then
#	Logging $TYPE $SEVERITY_CRITICAL $module "Aborting restart of device..."
#	printf "01\tAborting restart of device $device_name\n" >> /var/log/pluto/PlutoServer.log
#	wget -O /dev/null "www.1control.com/critical_error.php?installation=$INSTALLATION&Message=Device_${device_id}_failed"
#fi
