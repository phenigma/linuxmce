#!/bin/bash

#. /usr/pluto/bin/SQL_Ops.sh

Device_ID=""
Device_CmdLine=""
Device_Name=""
Device_Action=""

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                -d) ((i++)); Device_ID="${Params[$i]}" ;;
		-n) ((i++)); Device_Name="${Params[$i]}" ;;
		-S) Device_Action="start" ;;
		-K) Device_Action="stop" ;;
		-D) Device_Action="demonize" ;;
		--) ((i++)); Device_CmdLine=${Params[@]:$i} ; break ;;
        esac
done

function display_usage {
	echo "$(basename $0) [-d DeviceID]  <-S|-K> <-n DeviceName> [-- command line]"
}


function is_device_disabled {
	if [[ "$Device_ID" == "" ]] ;then
		return 1
	fi
	
	return 1
	#TODO: test if is disabled

}

if [[ "$Device_Name" == "" || "$Device_Action" == "" ]] ;then
	display_usage
	exit 1
fi

mkdir -p /var/run/pluto
mkdir -p /var/log/pluto

pidFile="/var/run/pluto/$Device_Name.pid"
if [[ "$Device_ID" == "" ]] ;then
	logFile="/var/log/pluto/${Device_Name}.log"
else
	logFile="/var/log/pluto/${Device_ID}_${Device_Name}.log"
fi

case "$Device_Action" in
	start)
		if [[ -f $pidFile ]] ;then
			testPid=$(cat $pidFile)
			if [[ -f /proc/$testPid/cmdline && "$testPid" != "" ]] ;then
				echo "Allready running on pid $testPid"
				exit 2
			fi
		fi

		screen -d -m -S "$Device_Name" $0 -n "$Device_Name" -d "$Device_ID" -D -- "$Device_CmdLine"
		echo "$$" > "$pidFile"
	;;

	demonize)
		no_of_restarts=0
		while [[ $no_of_restarts < 50 ]] ;do
			$Device_CmdLine >> $logFile
			err_code=$?

			if [[ "$err_code" == "3" ]] ;then
				echo "$(date)  Device shuting down" >> $logFile
				break

			elif [[ "$err_code" == "2" || "$err_code" == "0" ]] ;then
				echo "$(date)  Device requests restart" >> $logFile
				if is_device_disabled ;then
					echo "$(date) Device is disabled" >> $logFile
					break
				fi
			else
				echo "$(date)  Device died" >> $logFile
				((no_of_restarts++))
			fi
		done

		rm -f $pidFile
	;;


	stop)
	;;

	*)
		display_usage
		exit 1
	;;
esac
