#!/bin/bash

Message() {
    echo -en "\033[1m# $*"
    tput sgr0
}

StartService() {
	ServiceDescription=$1
	ServiceCmd=$2
	ServiceBkg=$3

	if [[ -x $(echo $ServiceCmd | cut -d ' ' -f1) ]] ;then
		echo -n "$ServiceDescription ... "
		if [[ "$ServiceBkg" == "&" ]] ;then
			$ServiceCmd 1>/dev/null 2>/dev/null &
		else
			$ServiceCmd 1>/dev/null 2>/dev/null	
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

	if [[ -x $(echo $ServiceCmd | cut -d ' ' -f1) ]] ;then
		echo -n "$ServiceDescription ... "
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

setterm -blank >/dev/console             # disable console blanking
chmod 777 /etc/pluto.conf 2>/dev/null    # ensure access rights on pluto.conf
rm /var/log/pluto/running.pids 2>/dev/null
chmod 777 /var/log/pluto 2>/dev/null
rm -f /dev/ttyS_*                        # remove all ttyS_* (created by gc100s) entries from /dev
mkdir -p /usr/pluto/locks                # clean up locks
rm -f /usr/pluto/locks/*                 # clean up locks
rm -f /var/run/plutoX*.pid		 # clean up x11 locks
rm -f /etc/rc{0,6}.d/S*{umountnfs.sh,portmap,networking}
invoke-rc.d nis start

#rm -f /etc/modprobe.d/lrm-video 


if [[ -f /usr/pluto/bin/Config_Ops.sh ]]; then
	 . /usr/pluto/bin/Config_Ops.sh
fi
if [[ -f /usr/pluto/bin/pluto.func ]] ;then
	. /usr/pluto/bin/pluto.func
fi
if [[ -f /usr/pluto/bin/SQL_Ops.sh ]] ;then
	. /usr/pluto/bin/SQL_Ops.sh
fi


export DISPLAY=":${Display}"

StartService "Loading Kernel Modules" "/usr/pluto/bin/LoadModules.sh"
if [[ -f /usr/pluto/bin/SQL_Ops.sh  && -f /usr/pluto/bin/Config_Ops.sh ]] ;then
	Q="SELECT FK_Installation FROM Device WHERE PK_Device='$PK_Device'"
	R="$(RunSQL "$Q")"
	ConfSet PK_Installation "$R"

	Q="SELECT PK_Users FROM Users LIMIT 1"
	R="$(RunSQL "$Q")"
	ConfSet PK_Users "$R"
fi


StartService "Setting SSH Keys" "/usr/pluto/bin/SSH_Keys.sh" "&"
StartService "Configure Device Changes" "/usr/pluto/bin/Config_Device_Changes.sh"
StartService "Setting Coredump Location" "/usr/pluto/bin/corefile.sh"
StartService "Creating Firewire 2 Video4Linux Pipes" "/usr/pluto/bin/Firewire2Video4Linux.sh"

# hack: cleaning lockfile on M/D start to allow
# local devices to start
# TODO: remove this when correct locking will be implemented
rm -f /usr/pluto/locks/pluto_spawned_local_devices.txt

#StartService "Starting Local Devices" "/usr/pluto/bin/Start_LocalDevices.sh"
StartService "Configuring Pluto Storage Devices" "/usr/pluto/bin/StorageDevices_Setup.sh" "&"
#StartDaemon "Report machine is on" "/usr/pluto/bin/Report_MachineOn.sh" "ReportingOn"

. /usr/pluto/bin/Config_Ops.sh
export DISPLAY=:${Display}
XPID=$(</var/run/plutoX$Display.pid)
if [[ -z "$XPID" || ! -d /proc/"$XPID" ]] ;then
	/usr/pluto/bin/Start_X.sh -config /etc/X11/xorg.conf
fi

export KDE_DEBUG=1
/usr/pluto/bin/lmce_launch_manager.sh
