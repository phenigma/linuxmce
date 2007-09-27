#!/bin/bash

LogFile="/var/log/startup-script.log"
exec &> >(tee $LogFile)


DisplayProgress() {
	Text=$1
	echo "${Progress}% ${Text}"
}

StartService() {
	ServiceDescription=$1
	ServiceCmd=$2
	ServiceBkg=$3
	Progress=$(( $Progress + $ProgressUnit ))


	if [[ -x $(echo $ServiceCmd | cut -d ' ' -f1) ]] ;then
		DisplayProgress "$ServiceDescription"
		echo -n "$ServiceDescription ..."
		if [[ "$ServiceBkg" == "&" ]] ;then
			$ServiceCmd &
		else
			$ServiceCmd
		fi
		err=$?

		if [[ "$err" == "0" ]] ;then
			echo "ok"
		else
			echo "fail"
		fi
	fi

	return $err
}

StartDaemon() {
	ServiceDescription=$1
	ServiceCmd=$2
	ServiceScreen=$3
	Progress=$(( $Progress + $ProgressUnit ))

	if [[ -x $(echo $ServiceCmd | cut -d ' ' -f1) ]] ;then
		DisplayProgress "$ServiceDescription"
		echo -n "$ServiceDescription ..."
		screen -d -m -S "$ServiceScreen" $ServiceCmd 
		err=$?

		if [[ "$err" == "0" ]] ;then
			echo "ok"
		else
			echo "fail"
		fi
	fi

	return $err
}

ThisFile=$0
Progress=0
ProgressChunks=$(grep "^ *StartService \|^ *StartDaemon " $ThisFile | wc -l)
ProgressUnit=$(( 100 / $ProgressChunks ))

setterm -blank >/dev/console             # disable console blanking
chmod 777 /etc/pluto.conf 2>/dev/null    # ensure access rights on pluto.conf
rm /var/log/pluto/running.pids 2>/dev/null
chmod 777 /var/log/pluto 2>/dev/null
rm -f /dev/ttyS_*                        # remove all ttyS_* (created by gc100s) entries from /dev
mkdir -p /usr/pluto/locks                # clean up locks
rm -f /usr/pluto/locks/*                 # clean up locks
rm -f /var/run/plutoX*.pid		 # clean up X11 locks
rm -f /mnt/optical/*.checksum


if [[ -f /usr/pluto/bin/Config_Ops.sh ]]; then
	 . /usr/pluto/bin/Config_Ops.sh
fi
if [[ -f /usr/pluto/bin/pluto.func ]] ;then
	. /usr/pluto/bin/pluto.func
fi
if [[ -f /usr/pluto/bin/SQL_Ops.sh ]] ;then
	. /usr/pluto/bin/SQL_Ops.sh
fi


StartService "Loading Kernel Modules" "/usr/pluto/bin/LoadModules.sh"

StartService "Setting SSH Keys" "/usr/pluto/bin/SSH_Keys.sh" "&"
StartService "Setting Coredump Location" "/usr/pluto/bin/corefile.sh"
StartService "Configuring Pluto Storage Devices" "/usr/pluto/bin/StorageDevices_Setup.sh" "&"
StartService "Status Radar" "/usr/pluto/bin/StorageDevices_StatusRadar.sh"
StartService "PVR-250 tuner restore" "/usr/pluto/bin/CaptureCards_BootConfig_PVR-250.sh"
