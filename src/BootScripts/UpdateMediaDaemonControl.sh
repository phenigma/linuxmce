#!/bin/bash
# UpdateMediaDaemonControl.sh - control UpdateMediaDaemon.sh script
#
# DESCRIPTION:
#  This script will control the daemon for UpdateMedia plugin.
#
# USAGE :
#  UpdateMediaDaemonControl.sh [ -enable | -disable | -status ]
#

function LogMsg ()
	{
		echo $(date -R) $*
	}

# wrong 

#if [[ [ -z "$1" ] || [[ "$1" != "-enable" ]] || [[ "$1" != "-disable" ]] || [[ "$1" != "-status" ]] ]]; then
#	LogMsg ": UpdateMediaDaemonControl : Wrong parameter supplied !"
#fi

if [[ "$1" == "-enable" ]];then
# enable option
        if [[ ! -z "$(pidof /usr/pluto/bin/UpdateMedia)" ]]; then
                LogMsg "UpdateMedia is allready running (PID $(pidof /usr/pluto/bin/UpdateMedia))"
        else
					
		LogMsg ": Enabling UpdateMediaDaemon on the system(s)"
		screen -d -m -S "BkgSS-UpdateMediaDaemon.sh" "/usr/pluto/bin/UpdateMediaDaemon.sh"
	fi

elif [[ "$1" == "-disable" ]];then
# disable option

	if [[ -z "$(pidof /usr/pluto/bin/UpdateMedia)" ]]; then
                LogMsg "UpdateMedia is stopped"
        else
							
		LogMsg ": Disabling UpdateMediaDaemon on the system(s)"
		UMD_scr_pid=$(ps auxw | grep -i screen | grep -i updatemedia | awk '{print $2}')	
		kill -9 $UMD_scr_pid
		rm -f /var/run/screen/S-root/*BkgSS-UpdateMediaDaemon.sh
	fi

elif [[ "$1" == "-status" ]]; then
# status option

	if [[ -z "$(pidof /usr/pluto/bin/UpdateMedia)" ]]; then
		LogMsg "UpdateMedia is stopped"
	else 
		LogMsg "UpdateMedia is running (PID - $(pidof /usr/pluto/bin/UpdateMedia))"
		
	fi

else 
# wrong param

	LogMsg ": UpdateMediaDaemonControl : Wrong parameter supplied !"
	LogMsg "Usage : UpdateMediaDaemonControl.sh [ -enable|-disable|-status ]"
fi

