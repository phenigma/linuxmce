#!/bin/bash
#
# Configuration script for : Device Template xxxx - Camera under Motion Wrapper
#

. /usr/pluto/bin/SQL_Ops.sh

## init variables

Device_ID=""
Device_RelationatedTo_ID=""
DeviceID=""
Device_IP=""
Device_MAC=""

MotionID=""
MotionCameraID=""
MotionCameraIDs=""

Mode=""

## end init var
WriteLog()
{

	LogFile="/var/log/pluto/Set_RelationsFor_MotionWrapper_Cameras.log"
	log=$1
	if [[ ! -f $LogFile ]];then
		touch $LogFile
	elif [[ ! -w $LogFile ]];then
		chmod +w $LogFile
		echo "================ New Log File ================" >> $LogFile
	else
		#echo "============= New Log =============" >> $LogFile
		echo "`date` :: $log" >> $LogFile
		#echo "Error Writen to LogFile : $LogFile"
	fi
}
		
Usage()
{
        echo "Usage: $0 {--manual <Device_ID> [<Device_RelationatedTo_ID>] | --auto | --help}"
	WriteLog "Incorect usage of the script!!!"
        exit 0
}


## Get Motion ID

Q="
	SELECT PK_Device
	FROM Device
	WHERE FK_DeviceTemplate=64;
  "
MotionID=$(RunSQL "$Q")

## Auto Function = set all Motion Cameras related to themselfs

Auto()
{
	Q="
        	SELECT PK_Device
	        FROM Device
	        WHERE FK_Device_ControlledVia='$MotionID';
	  "

	MotionCameraIDs=$(RunSQL "$Q")

        for row in $MotionCameraIDs;do
		MotionCameraID=$(Field 1 "$row")
		WriteLog "$MotionCameraID"
		
		RunSQL "DELETE FROM Device_Device_Related WHERE FK_Device=$MotionCameraID AND FK_Device_Related=$MotionCameraID"
		RunSQL "INSERT INTO Device_Device_Related (FK_Device, FK_Device_Related) VALUES ($MotionCameraID, $MotionCameraID)"
		WriteLog "Inserted FK_Device=`$MotionCameraID` and FK_Device_Related=`$MotionCameraID` in Device_Device_Related table"
	done        

}

## Manual Function = set Sensor related to Camera acording to input parameters

Manual()
{
	Q="
		SELECT FK_DeviceTemplate
		FROM Device
		WHERE PK_Device='$Device_ID';
	  "

	test=$(RunSQL "$Q")

	if [[ -z "$test" ]]; then
		WriteLog "There is no device with DeviceID = $Device_ID"
		exit 0
			
	else 
		if [[ $Mode = "manual2" ]]; then
	
			Q="
                        	SELECT FK_DeviceTemplate
	                        FROM Device
        	          	WHERE PK_Device='$Device_RelationatedTo_ID';
                	  "
		
			test=$(RunSQL "$Q")
		
			if [[ -n "$test" ]]; then
		
				RunSQL "DELETE FROM Device_Device_Related WHERE FK_Device=$Device_ID AND FK_Device_Related=$Device_RelationatedTo_ID"
		                RunSQL "INSERT INTO Device_Device_Related (FK_Device, FK_Device_Related) VALUES ($Device_ID, $Device_RelationatedTo_ID)"
       			WriteLog "Inserted FK_Device= $Device_ID and FK_Device_Related= $Device_RelationatedTo_ID in Device_Device_Related table"

			else
				WriteLog "There is no device_related with DeviceID = $Device_RelationatedTo_ID"
				exit 0
			fi

		elif [[ $Mode = "manual1" ]]; then
		
			RunSQL "DELETE FROM Device_Device_Related WHERE FK_Device=$Device_ID AND FK_Device_Related=$Device_ID"
                        RunSQL "INSERT INTO Device_Device_Related (FK_Device, FK_Device_Related) VALUES ($Device_ID, $Device_ID)"
                        WriteLog "Inserted FK_Device= $Device_ID and FK_Device_Related= $Device_ID in Device_Device_Related table"

		else
			WriteLog "No valid Manual Mode"
			exit 0
		fi
	
	fi

}

## ConfigDevice function

ConfDev()
{


	Q="
		SELECT FK_DeviceTemplate
		FROM Device
		WHERE PK_Device='$Device_ID';
	  "

	test=$(RunSQL "$Q")

	if [[ -z "$test" ]]; then
		WriteLog "There is no device with DeviceID= $Device_ID"
		exit 0
			
	else 
		RunSQL "DELETE FROM Device_Device_Related WHERE FK_Device=$Device_ID AND FK_Device_Related=$Device_ID"
                RunSQL "INSERT INTO Device_Device_Related (FK_Device, FK_Device_Related) VALUES ($Device_ID, $Device_ID)"
                WriteLog "Inserted FK_Device= $Device_ID and FK_Device_Related= $Device_ID in Device_Device_Related table"
	fi
}
	
## Test input parameters
if [[ "$#" == 0 ]]; then
        Usage
fi

for ((i = 1; i <= "$#"; i++)); do
        case "${!i}" in
               --auto) Mode="auto"
			WriteLog "--- Auto Mode ---"
              		Auto;; 
	       --manual)
                       if [[ "$#" == 1 ]]; then
			  WriteLog "Not enought parameters for Manual Mode"
			  Usage
			  
			  elif [[ "$#" == 2 ]]; then
				Mode="manual1"
				((i++)); Device_ID="${!i}"
  			         Device_RelationatedTo_ID=$Device_ID
				 WriteLog "--- Manual Mode 1 ---"
				 WriteLog "Device_ID= $Device_ID, Device_RelationatedTo_ID = $Device_ID"
				Manual
			   elif [[ "$#" == 3 ]]; then
			        Mode="manual2"
				((i++)); Device_ID="${!i}"
				((i++)); Device_RelationatedTo_ID="${!i}"
				WriteLog "--- Manual Mode 2 ---"
                                WriteLog "Device_ID= $Device_ID, Device_RelationatedTo_ID = $Device_ID"
				Manual
			else
				WriteLog "Too Many parameters for Manual Mode"
				Usage

			fi
		;;
		-d)
			#echo " ---- $# --- => $1 $2 $3 $4 $5 $6"
			((i++)); Device_ID="${!i}"
			((i=i+2)); Device_IP="${!i}" # -i
			((i=i+2)); Device_MAC="${!i}" # -m
			WriteLog "--- Configure Device Mode ---"
			WriteLog "Device_ID=$Device_ID, Device_IP=$Device_IP, Device_MAC=$Device_MAC"
			ConfDev
		;;
                --help|*) Usage ;;
        esac
done




