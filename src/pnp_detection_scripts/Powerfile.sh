#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

echo "Yamaha Detection script queue $2"
echo "Works for AV Receivers"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
exit

FK_Manufacturer="318"
FK_DeviceCategory="103"
FK_DeviceData="59" # this is where the Model ID is kept for serial devices

ManufactReadyCommand="\s1000m\11000\03\s1000m\11000\03"
ManufactConfigRecon="\12"


GsdResponse=`/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t ${ManufactReadyCommand} -i 3 -s ${ManufactConfigRecon}`

if [[ "$?" -ne 0 ]]; then
	echo "It's not a Yamaha Receiver"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
	echo "It is a Yamaha Receiver"

	ModelId=${GsdResponse:3:5}
	echo "Querying the database for device template matching model ID ${ModelId}"
	
	Q="SELECT FK_DeviceTemplate 
			FROM DeviceTemplate_DeviceData
				JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_Manufacturer=${FK_Manufacturer} AND FK_DeviceData=59 AND FK_DeviceCategory=${FK_DeviceCategory}
					AND IK_DeviceData = '${ModelId}'"

	R=$(RunSQL "$Q")

	if [[ -z $R ]]; then
		echo "The specific model dosen't have a template associated with it, trying to use a default template \
			from the same manufacturer and device category"

		Q="SELECT PK_DeviceTemplate
		        FROM DeviceTemplate
				WHERE FK_Manufacturer=${FK_Manufacturer} AND FK_DeviceCategory=${FK_DeviceCategory} AND FK_CommMethod=2"
		R=$(RunSQL "$Q")
	fi

	if [[ -z $R ]];	then
	    echo "Could not find a suitable device from this manufacturer and within this category"
	    /usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
	    return 0
	fi

	ActualModelDeviceTemplate=${R%% *}
	/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 $ActualModelDeviceTemplate
fi
