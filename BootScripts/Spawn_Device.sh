#!/bin/bash

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
new_log="$log_file.newlog"
real_log="$log_file.log"

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

for i in $(seq 1 10); do
	Logging $TYPE $SEVERITY_NORMAL "$module" "Appending log..."
	cat "$new_log" >> "$real_log"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i"
	echo $(date) Starting > "$new_log"

	Logging $TYPE $SEVERITY_NORMAL "$module" "Found $Path$cmd_line"
	"$Path$cmd_line" -d "$device_id" -r "$ip_of_router" | tee "$new_log"
	
	if [ -f /var/pluto/bootpluto/shutdown_$device_id -o -f /var/tmp/shutdown_$device_id ];
	then
		Logging $TYPE $SEVERITY_NORMAL "$module" "Shutting down... $i $device_name"
		echo $(date) Shutdown >> "$new_log"
		exit
	else
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... $i $device_name"
		echo $(date) died >> "$new_log"
		echo $(date) $device_name died >> /var/log/pluto/deaths
		echo $(date) died >> /var/log/pluto/core/died_${device_id}_$device_name
	fi
	echo out
done
#if [ "$i" -eq 10 ]; then
#	Logging $TYPE $SEVERITY_CRITICAL $module "Aborting restart of device..."
#	printf "01\tAborting restart of device $device_name\n" >> /var/log/pluto/PlutoServer.log
#	wget -O /dev/null "www.1control.com/critical_error.php?installation=$INSTALLATION&Message=Device_${device_id}_failed"
#fi
