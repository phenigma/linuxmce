#!/bin/bash

# The usb serial ports use to be stored like this in the database before 2.0.40 :
# 	usb1/1-2/1-2:1.0
# From now they are stored like this :
#	pci0000:00/0000:00:02.0+2
# This script old format to the new fromat in an allready existing database 

if [[ -f /usr/pluto/bin/SQL_Ops.sh ]] ;then
	. /usr/pluto/bin/SQL_Ops.sh
else
	echo "SQL_Ops.sh not present, must be an fresh system, nothing to update."
	exit 0
fi

Q="
	SELECT
		FK_Device,
		FK_DeviceData,
		IK_DeviceData
	FROM
		Device_DeviceData
	WHERE
		IK_DeviceData LIKE 'usb%'
	AND
		FK_DeviceData IN ( 37 )
"
R=$(RunSQL "$Q")

for Line in $R ;do
	FK_Device=$(Field "1" "$Line")
	FK_DeviceData=$(Field "2" "$Line")
	IK_DeviceData=$(Field "3" "$Line")

	## Extract usb? and port number from the old database device data
	usb_controler=$( echo $IK_DeviceData | sed 's|\(usb[0-9]*\)/[0-9]*-[0-9]*/.*|\1|' )
	usb_port=$( echo $IK_DeviceData | sed 's|usb[0-9]*/[0-9]*-\([0-9]*\)/.*|\1|' )

	if [[ $usb_port == "" || $usb_controler == "" ]] ;then
		echo "Warning: can't extract usb controler id and port id from $IK_DeviceData ($FK_Device $FK_DeviceData)..."
	fi

	## Try to translate usb? into a pci id using sysfs
	if [[ -r /sys/bus/usb/devices/$usb_controler ]] ;then
		usb_pciid=$( readlink -f /sys/bus/usb/devices/$usb_controler | sed 's|.*/devices/\(pci.*\)/usb[0-9]*|\1|' )
	else
		echo "Warning: can't extract pciid of $usb_controler ..."
		continue
	fi

	echo "Translating: $IK_DeviceData -> $usb_pciid+$usb_port"
	Q="
	UPDATE 	Device_DeviceData 
	SET 
		IK_Device = '$usb_pciid+$usb_port'
	WHERE
		FK_Device = '$FK_Device'
		AND
		FK_DeviceData = '$FK_DeviceData'
	"
	#RunSQL "$Q"
done
