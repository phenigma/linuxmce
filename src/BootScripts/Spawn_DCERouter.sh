#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

cd /usr/pluto/bin
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting DCERouter"

log_file="/var/log/pluto/DCERouter";
new_log="$log_file.newlog"
real_log="$log_file.log"

module=DCERouter
device_name="$module"

i=1
while [ "$i" -le 10 ]; do
	Logging $TYPE $SEVERITY_NORMAL "$module" "Appending log..."
	cat "$new_log" >> "$real_log"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i"
	echo $(date) Starting > "$new_log"

	/usr/pluto/bin/UpdateEntArea -h localhost > >(tee $new_log)
	/usr/pluto/bin/Spawn_Wrapper.sh $VGcmd/usr/pluto/bin/DCERouter -h localhost > >(tee $new_log)

	Ret="$?"
	echo "Return code: $Ret" >>"$new_log"
	if [ "$Ret" -eq 3 ]; then
		# Abort
		Logging $TYPE $SEVERITY_NORMAL "$module" "Shutting down... $i $device_name"
		echo $(date) Shutdown >> "$new_log"
		exit
	elif [ "$Ret" -eq 2 ]; then
		Logging $TYPE $SEVERITY_NORMAL "$module" "Device requests restart... $i $device_name"
		/usr/pluto/bin/Start_LocalDevices.sh
		echo $(date) Shutdown >> "$new_log"
	else
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... $i $device_name"
		echo $(date) died >> "$new_log"
		echo $(date) $device_name died >> /var/log/pluto/deaths
		echo $(date) died >> /var/log/pluto/died_${device_id}_$device_name
		sleep 5
		/usr/pluto/bin/Start_LocalDevices.sh
		i=$((i+1))
	fi
	echo out
done

# old
#screen -d -m -S DCERouter bash -c "(echo \$(date) Starting; $VGcmd/usr/pluto/bin/DCERouter -h localhost) | tee $new_log"
#timeout=60
#waited=0
#while ! nc -z localhost 3450 &>/dev/null && [ "$waited" -lt "$timeout" ]; do
#	sleep 1
#	waited=$((waited + 1))
#done
#if [ "$waited" -eq "$timeout" ]; then
#	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "--> DCERouter failed to start"
#	exit 1
#else
#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "DCERouter started ok"
#	PID=$(ps ax | grep /usr/pluto/bin/DCERouter | egrep -v 'SCREEN|grep|bash' | awk '{ print $1 }')
#	echo "$PID DCERouter (by $0)" >>/var/log/pluto/running.pids
#fi
