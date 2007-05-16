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
		$ServiceCmd 1>/dev/null 2>/dev/null $ServiceBkg
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
rm -f /etc/rc{0,6}.d/S*{umountnfs.sh,portmap,networking}
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
if [[ "$AVWizardDone" != "1" ]] ;then
	StartService "Starting Audio/Video Wizard" "/usr/pluto/bin/AVWizard_Run.sh"
fi
StartService "Starting X11 Server" "/usr/pluto/bin/Start_X.sh"
StartService "Starting the Launch Manager" "/usr/pluto/bin/lmce_launch_manager"
StartService "Creating Firewire 2 Video4Linux Pipes" "/usr/pluto/bin/Firewire2Video4Linux.sh"
#StartService "Starting Local Devices" "/usr/pluto/bin/Start_LocalDevices.sh"
StartService "Configuring Pluto Storage Devices" "/usr/pluto/bin/StorageDevices_Setup.sh" "&"
StartService "Report machine is on" "/usr/pluto/bin/Report_MachineOn.sh" "&"

#Pluto_alsaconf-noninteractive \
#Pluto_Diskless_Setup.sh \
#Pluto_Restart_DHCP.sh \
#Pluto_Restart_MythBackend.sh \
#Pluto_Setup_ExportsNFS.sh \
#Pluto_VoiceMailMonitor.sh \
#Pluto_kpanic.sh \
#Pluto_qos.pl \
#Pluto_Report_MachineOn.sh \
#Pluto_Net_Mount_All.sh \
#Pluto_WakeMDs.sh \
#Pluto_Backup_Database.sh \
#Pluto_Share_IRCodes.sh \

