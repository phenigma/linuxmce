#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

#flag='0'
echo >> /DeviceDied.log
echo "*DEVICES DIED DURING THE TESTS*" >> /DeviceDied.log
echo "===============================" >> /DeviceDied.log
echo "deviced died" >> /tmp/status.log
echo >> /DeviceDied.log

dateToday=$(date +'%a %b %e' | sed 's/  / /')
dateYesterday=$(date --date=yesterday +'%a %b %e' | sed 's/  / /')

for logFile in /var/log/pluto/died_* ;do
	logFile=`basename $logFile`

	## Device ID
	deviceID=${logFile#*_*_}
	if [[ "$deviceID" == "DCERouter" ]] ;then 
		deviceDescription="DCERouter"
	else	
		## Device description
		Q="SELECT Description FROM Device WHERE PK_Device='$deviceID'"
		R=$(RunSQL "$Q")
		if [[ $R == "" ]]; then
			continue
		fi
		deviceDescription=$(Field 1 "$R")
	fi

	## Now many times the device started
	# 0:00 -> 9:59 Today
	deviceStarted_Today=$(cat /var/log/pluto/$logFile | grep "^$dateToday *[0-9]" | wc -l )
	
	# 23:00 -> 24:00 Yesterday
	#deviceStarted_Yesterday=0
	deviceStarted_Yesterday=$(cat /var/log/pluto/$logFile | grep "^$dateYesterday 2[3-4]" | wc -l)
	
	deviceStarted=$(( $deviceStarted_Today + $deviceStarted_Yesterday ))
	
	## Generate the device statistics
	echo "Device   : $deviceID ($deviceDescription) " >> /DeviceDied.log
	echo "Logfile  : $logFile" >> /DeviceDied.log
	echo "Died     : $deviceStarted times" >> /DeviceDied.log
	echo >> /DeviceDied.log
done

echo "More details: " >> /DeviceDied.log
echo >> /DeviceDied.log

while read line ;do 
	#if [[ "$flag" == '0' ]] ;then
	#	if [[ "$line" == *"---STARTING LOOP SCRIPT--"* ]] ;then	
	#		flag='1'
	#	fi
	#else
		if [[ "$line" == *"Device died"* ]] ;then
			echo $line >> /DeviceDied.log 
		fi
	#fi
done < /var/log/pluto/pluto.log
