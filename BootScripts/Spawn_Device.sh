#!/bin/bash

mkdir -p /var/log/pluto
echo "$(date) Spawn_Device $*" >> /var/log/pluto/Spawn_Device.log

# syntax: $0 <device_id> <ip_of_router> <display> <cmd_line>

. /pluto/bin/pluto.func || exit
device_id="$1"
ip_of_router="$2"
device_name="$device_id"
module="$device_name" #$(basename $0)
display="$3"
#cmd_line="$4"
cmd_line="$3"

Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "device: $device_id ip: $ip_of_router display: $display cmd_line: $cmd_line"

log_file="/var/log/pluto/${device_id}_${cmd_line}"
new_log="$log_file.newlog"
real_log="$log_file.log"

if [ -z "$cmd_line" ]; then
	Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "Empty command line"
	exit 1
fi

if [ ! -x "$cmd_line" ]; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0 $module" "Can't execute $cmd_line"
	exit 1
fi

if [ -z "$display" ]; then
	display=":0"
fi

#export DISPLAY="$display"
export DISPLAY=:0

Logging $TYPE $SEVERITY_STAGE $module "Entering $module"
for i in $(seq 1 10); do
	Logging $TYPE $SEVERITY_NORMAL $module "Appending log..."
	cat "$new_log" >> "$real_log"
	Logging $TYPE $SEVERITY_NORMAL $module "Starting... $i"
	echo $(date) Starting > "$new_log"

	if [ -x "./$cmd_line" ]; then
		echo "$(date) Found ./$cmd_line"
		"./$cmd_line" -d "$device_id" -r "$ip_of_router" | tee "$new_log"
	elif [ -x "/pluto/bin/$cmd_line" ]; then
		echo "$(date) Found /pluto/bin/$cmd_line"
		"/pluto/bin/$cmd_line" -d "$device_id" -r "$ip_of_router" | tee "$new_log"
	else
		echo "$(date) Not Found $cmd_line"
		true
	fi
	
	if [ -f /var/pluto/bootpluto/shutdown_$device_id -o -f /var/tmp/shutdown_$device_id ]
	then
		Logging $TYPE $SEVERITY_NORMAL $module "Shutting down... $i $device_name"
		echo $(date) Shutdown >> "$new_log"
		exit
	else
		Logging $TYPE $SEVERITY_CRITICAL $module "Device died... $i $device_name"
		echo $(date) died >> "$new_log"
		echo $(date) $device_name died >> /var/log/pluto/deaths
		echo $(date) died >> /var/log/pluto/core/died_${device_id}_$device_name
	fi
	sleep 10
done
#if [ "$i" -eq 10 ]; then
#	Logging $TYPE $SEVERITY_CRITICAL $module "Aborting restart of device..."
#	printf "01\tAborting restart of device $device_name\n" >> /var/log/pluto/PlutoServer.log
#	wget -O /dev/null "www.1control.com/critical_error.php?installation=$INSTALLATION&Message=Device_${device_id}_failed"
#fi
