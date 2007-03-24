#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/CaptureCards_Utils.sh

DEVICECATEGORY_Capture_Cards=75
DEVICETEMPLATE_WinTV_PVR_USB_2=1874
DEVICETEMPLATE_GeneralInfoPlugin=27
DEVICEDATA_Location_on_PCI_bus=175
DEVICEDATA_Setup_Script=189

CaptureCard_PCISlot()
{
	local PK_Device_CC="$1"
	local Q Slot

	if [[ -z "$PK_Device_CC" ]]; then
		return 1
	fi

	Q="
		SELECT IK_DeviceData
		FROM Device_DeviceData
		WHERE FK_Device='$PK_Device_CC' AND FK_DeviceData='$DEVICEDATA_Location_on_PCI_bus'
	"
	Slot="$(RunSQL "$Q")"
	Slot="${Slot##*/}"
	Slot="${Slot#*:}"

	echo "$Slot"
	return 0
}

OurDevice()
{
	local Device="$1" Slot="$2" ID="$3"
	local Q R Template

	if [[ -z "$Slot" || -z "$ID" ]]; then
		return 1
	fi

	Q="
		SELECT COUNT(VendorModelID)
		FROM DHCPDevice
		JOIN Device ON Device.FK_DeviceTemplate=DHCPDevice.FK_DeviceTemplate
		WHERE
			PK_Device='$Device'
			AND '$ID' LIKE CONCAT(VendorModelID,'%')
	"
	R=$(RunSQL "$Q")

	[[ "$R" != 0 ]]
	return $?
}

DisableDevice()
{

	local Device="$1"
	local Q

	if [[ -z "$Device" ]]; then
		return 1
	fi
	
	# COMMAND: #791 - Set Enable Status
	GeneralInfoPlugin_DeviceID=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_GeneralInfoPlugin" "" "")
	/usr/pluto/bin/MessageSend dcerouter 0 $GeneralInfoPlugin_DeviceID 1 791 2 $Device 208 0

}

EnableDevice()
{
	local Device="$1"
	local Q CmdLine

	if [[ -z "$Device" ]]; then
		return 1
	fi

	# COMMAND: #791 - Set Enable Status
	GeneralInfoPlugin_DeviceID=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_GeneralInfoPlugin" "" "")
	/usr/pluto/bin/MessageSend dcerouter 0 $GeneralInfoPlugin_DeviceID 1 791 2 $Device 208 1

	if [[ "$Slot" != "" ]] ;then
		Q="
			SELECT DDDD.IK_DeviceData
			FROM DeviceTemplate_DeviceData DDDD
			JOIN Device D ON D.FK_DeviceTemplate=DDDD.FK_DeviceTemplate
				AND D.PK_Device='$Device'
				AND DDDD.FK_DeviceData='$DEVICEDATA_Setup_Script'
		"
		CmdLine=$(RunSQL "$Q")
	
		if [[ -n "$CmdLine" ]]; then
			/usr/pluto/bin/"$CmdLine" "$Device" "$Slot"
			# card script will also update ports
		else
			UpdatePorts "$Device" "$Slot"
		fi
	fi
}

CaptureCards=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Capture_Cards" "" "" "all")

for CardDevice in $CaptureCards; do


	CardDevice_Template=$(RunSQL "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = $CardDevice")
	echo "--> Setting up card '$CardDevice'"

	# PVRUSB2 capture card
	if [[ "$CardDevice_Template" == "$DEVICETEMPLATE_WinTV_PVR_USB_2" ]] ;then
		CardDevice_PciID=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $CardDevice AND FK_DeviceData = $DEVICEDATA_Location_on_PCI_bus")
		if [[ -z "$CardDevice_PciID" ]];then
			continue
		fi

		CardDevice_HalUDI=$(hal-find-by-property --key linux.sysfs_path --string "/sys/devices/$CardDevice_PciID")
		if [[ -z "$CardDevice_HalUDI" ]] ;then 
			DisableDevice "$CardDevice"
			continue
		fi

		CardDevice_UsbSerial=$(hal-get-property --udi $CardDevice_HalUDI --key usb_device.serial)
		if [[ -z "$CardDevice_UsbSerial" ]] ;then
			DisableDevice "$CardDevice"
			continue
		fi

		Port=""
		for dir in /sys/class/pvrusb2/* ;do
			if [[ "$(cat $dir/device/serial)" == "$CardDevice_UsbSerial" ]]; then
				Port="video$(cat $dir/v4l_minor_number)"
			fi
		done

		if [[ "$Port" != "" ]] ;then
			Slot=""
			EnableDevice "$CardDevice"
			UpdatePorts_NoFind "$CardDevice" "$Port"
		else
			DisableDevice "$CardDevice"
		fi	
	# Generic capture card on PCI
	else
		Slot=$(CaptureCard_PCISlot "$CardDevice")
		if [[ -z "$Slot" ]]; then
			continue
		fi

	
		ID="$(lshwd -id | grep "^$Slot" | cut -d' ' -sf2)"
		ID="${ID//:/}"
		
		if ! OurDevice "$CardDevice" "$Slot" "$ID"; then
			DisableDevice "$CardDevice"
		else
			EnableDevice "$CardDevice"
		fi
	fi
done
