#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
DD_DHCP=28;

# Increments an IP addrees by 1
function IncIP() {
	local Address="$1"
	Address_1=$(echo "$Address" | cut -d'.' -f1)
	Address_2=$(echo "$Address" | cut -d'.' -f2)
	Address_3=$(echo "$Address" | cut -d'.' -f3)
	Address_4=$(echo "$Address" | cut -d'.' -f4)

	if [[ $Address_4 -lt 255 ]] ;then
		Address_4=$(( Address_4 + 1 ))
	elif [[ $Address_3 -lt 255 ]] ;then
		Address_3=$(( Address_3 + 1 ))
		Address_4=0
	elif [[ $Address_2 -lt 255 ]] ;then
		Address_2=$(( Address_2 + 1 ))
		Address_3=0
		Address_4=0
	elif [[ $Address_1 -lt 255 ]] ;then
		Address_1=$(( Address_1 + 1 ))
		Address_2=0
		Address_3=0
		Address_4=0
	fi
	
	echo "$Address_1.$Address_2.$Address_3.$Address_4"
}

# Read the starting ip address for the devices
R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData where FK_Device=1 AND FK_DeviceData=$DD_DHCP")
DHCP_Range=$(Field 1 "$R")
DHCP_Range=$(echo $DHCP_Range | cut -d',' -f1)
DHCP_Start=$(echo $DHCP_Range | cut -d'-' -f1)

# Update the devices that have an ip address assigned excepting the core
R=$(RunSQL "SELECT PK_Device FROM Device WHERE IPaddress <> '' AND PK_Device <> 1");
Next_IP=$DHCP_Start
for Device in $R ;do
	Device_ID=$(Field 1 "$Device");
	RunSQL "UPDATE Device SET IPaddress='$Next_IP' WHERE PK_Device='$Device_ID' LIMIT 1"
	
	Next_IP=$(IncIP $Next_IP)
done

# Mark all Phones as needing configure
RunSQL "UPDATE Device SET NeedConfigure = 1 WHERE FK_DeviceTemplate IN ($(ListTemplates_Category 89 | sed 's/ /,/g'))"
/usr/pluto/bin/Config_Device_Changes.sh

# Mark DisklessMDs as needing configure
RunSQL "UPDATE Device SET NeedConfigure=1 WHERE FK_DeviceTemplate=28"


# We should not run this
/usr/pluto/bin/StorageDevices_ExportsNFS.sh
/usr/pluto/bin/StorageDevices_ExportsSMB.sh
/usr/pluto/bin/Diskless_Setup.sh
