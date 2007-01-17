#!/bin/bash

Message() {
    echo -e "\033[1m# $*"
    tput sgr0
}

## Start stuff that makes the dcerouter act as a internet router
if [[ -x /etc/init.d/dhcp3-server ]] ;then
	/etc/init.d/dhcp3-server start 1>/dev/null 2>/dev/null &
fi
if [[ -x /etc/init.d/bind9 ]] ;then
	/etc/init.d/bind9 start 1>/dev/null 2>/dev/null &
fi

#Start MySQL Server
if [[ -x /etc/init.d/mysql ]] ;then
	Message "Starting MySQL Server"
	/etc/init.d/mysql start
fi

if [[ -x /usr/pluto/bin/Network_Firewall.sh ]] ;then
	/usr/pluto/bin/Network_Firewall.sh 1>/dev/null 2>/dev/null &
fi

if [[ -f /usr/pluto/bin/Config_Ops.sh ]]; then
	 . /usr/pluto/bin/Config_Ops.sh
fi
if [[ -f /usr/pluto/bin/pluto.func ]] ;then
	. /usr/pluto/bin/pluto.func
fi
if [[ -f /usr/pluto/bin/SQL_Ops.sh ]] ;then
	. /usr/pluto/bin/SQL_Ops.sh
fi

## Load the modules listed media director's device data
if [[ -x /usr/pluto/bin/LoadMDModules.sh ]] ;then
	/usr/pluto/bin/LoadMDModules.sh
fi

# assure some settings
if [[ -x /usr/pluto/bin/Report_Machine_Status.sh ]]; then
	/usr/pluto/bin/Report_Machine_Status.sh  &
fi

if [[ -f /usr/pluto/bin/SQL_Ops.sh  && -f /usr/pluto/bin/Config_Ops.sh ]] ;then
	Q="SELECT FK_Installation FROM Device WHERE PK_Device='$PK_Device'"
	R="$(RunSQL "$Q")"
	ConfSet PK_Installation "$R"

	Q="SELECT PK_Users FROM Users LIMIT 1"
	R="$(RunSQL "$Q")"
	ConfSet PK_Users "$R"
fi

setterm -blank >/dev/console             # disable console blanking
chmod 777 /etc/pluto.conf 2>/dev/null    # ensure access rights on pluto.conf
rm /var/log/pluto/running.pids 2>/dev/null
chmod 777 /var/log/pluto 2>/dev/null
rm -f /dev/ttyS_*                        # remove all ttyS_* (created by gc100s) entries from /dev
mkdir -p /usr/pluto/locks                # clean up locks
rm -f /usr/pluto/locks/*                 # clean up locks
rm -f /etc/rc{0,6}.d/S*{umountnfs.sh,portmap,networking}

if [[ "$FirstBoot" != "false" && ! -f /usr/pluto/install/.notdone ]] ;then
	if [[ ! -f /etc/diskless.conf ]] ;then
		Message "Updating software database (getxmls)"
		/usr/pluto/bin/getxmls
	fi

	ConfSet "FirstBoot" "false" 2>/dev/null 1>/dev/null
fi

if [[ -x /usr/pluto/bin/SSH_Keys.sh ]] ;then 
	Message "Setting ssh keys"
	/usr/pluto/bin/SSH_Keys.sh &
fi

if [[ -x /usr/pluto/bin/ConfirmInstallation.sh ]] ;then
	Message "Confirm Instalation"
	/usr/pluto/bin/ConfirmInstallation.sh
fi

Message "Running depmod"
/sbin/depmod

if [ -x /usr/pluto/bin/Start_DCERouter.sh ] ;then
	Message "Starting DCE Router"
	/usr/pluto/bin/Start_DCERouter.sh
fi

if [ -x /usr/pluto/bin/Config_Device_Changes.sh ] ;then
	Message "Configure Device Changes"
	/usr/pluto/bin/Config_Device_Changes.sh
fi

if [[ -x /usr/pluto/bin/shift_state ]]; then
	ShiftState=$(/usr/pluto/bin/shift_state) # Shift=1; Ctrl=4; LAlt=8; RAlt=2; -- OR flags
	if [[ "$ShiftState" == 1 ]]; then
		ShiftMsg=" (shift pressed)"
	fi
fi

if [[ -x /usr/pluto/bin/corefile.sh ]]; then
	Message "Setting core file"
	/usr/pluto/bin/corefile.sh
fi

Message "Starting Pluto HAL daemon"
/usr/pluto/bin/StartHAL.sh

if [[ "$AVWizardDone" != "1" || "$ShiftState" == 1 ]] ;then
	if [[ -x /usr/pluto/bin/AVWizard_Run.sh ]]; then	
		Message "Starting Audio/Video Wizard${ShiftMsg}"
		/usr/pluto/bin/AVWizard_Run.sh
	fi
fi

if [ -x /usr/pluto/bin/Start_OrbiterGen.sh ] ;then
	Message "Starting OrbiterGen"
	/usr/pluto/bin/Start_OrbiterGen.sh
fi

if [ -x /usr/pluto/bin/Start_X.sh ] ;then
	Message "Starting X Server"
	/usr/pluto/bin/Start_X.sh
fi

if [ -x /usr/pluto/bin/Firewire2Video4Linux.sh ] ;then
	Message "Creating Firewire 2 Video4Linux Pipes"
	/usr/pluto/bin/Firewire2Video4Linux.sh
fi

if [ -x /usr/pluto/bin/Start_LocalDevices.sh ] ;then
	Message "Starting Local Devices"
	/usr/pluto/bin/Start_LocalDevices.sh
fi

if [[ -x /usr/pluto/bin/StorageDevices_Setup.sh ]]; then
	/usr/pluto/bin/StorageDevices_Setup.sh 1>/dev/null 2>/dev/null &
fi


