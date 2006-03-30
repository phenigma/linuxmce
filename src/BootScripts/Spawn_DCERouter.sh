#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/PlutoVersion.h

Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "Writing Version: $Version"
echo "$Version" >/home/pluto-version

cd /usr/pluto/bin
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting DCERouter"

LogFile="/var/log/pluto/DCERouter.log";

module=DCERouter
device_name="$module"

i=1
while [ "$i" -le 200 ]; do
	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i"
	echo $(date) Starting >>"$LogFile"

	/usr/pluto/bin/UpdateEntArea -h localhost > >(tee -a /var/log/pluto/updateea.log)
	mysqldump pluto_main Device > /var/log/pluto/device.$(date +%F-%T)
	if [ "${Valgrind/DCERouter}" != "$Valgrind" ]; then
		/usr/pluto/bin/Spawn_Wrapper.sh $VGcmd/usr/pluto/bin/DCERouter -h localhost &> >(tee -a $LogFile)
	else
		/usr/pluto/bin/Spawn_Wrapper.sh /usr/pluto/bin/DCERouter -h localhost &> >(tee -a $LogFile)
	fi

	Ret="$?"
	echo "Return code: $Ret" >>"$LogFile"
	if [ "$Ret" -eq 3 ]; then
		# Abort
		Logging $TYPE $SEVERITY_NORMAL "$module" "Shutting down... $i $device_name"
		echo $(date) Shutdown >>"$LogFile"
		exit
	elif [ "$Ret" -eq 2 ]; then
		Logging $TYPE $SEVERITY_NORMAL "$module" "Device requests restart... $i $device_name"
		#/usr/pluto/bin/Start_LocalDevices.sh
		echo $(date) Restart >>"$LogFile"
	else
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... $i $device_name"
		echo $(date) died >>"$LogFile"
		echo $(date) $device_name died >> /var/log/pluto/deaths
		echo $(date) died >> /var/log/pluto/died_${device_id}_$device_name
		sleep 5
		#/usr/pluto/bin/Start_LocalDevices.sh
		i=$((i+1))
	fi
	echo out
done

# old
#screen -d -m -S DCERouter bash -c "(echo \$(date) Starting; $VGcmd/usr/pluto/bin/DCERouter -h localhost) | tee -a $LogFile"
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
