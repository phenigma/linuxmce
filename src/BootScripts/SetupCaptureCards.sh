#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICECATEGORY_Capture_Cards=75
DEVICEDATA_Location_on_PCI_bus=175

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

	Q="
		UPDATE Device
		SET Disabled=1
		WHERE PK_Device='$Device'
	"
	RunSQL "$Q"
}

EnableDevice()
{
	local Device="$1"
	local Q CmdLine

	if [[ -z "$Device" ]]; then
		return 1
	fi

	Q="
		UPDATE Device
		SET Disabled=0
		WHERE PK_Device='$Device'
	"
	RunSQL "$Q"

	Q="
		SELECT CommandLine
		FROM DeviceTemplate
		JOIN Device ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE PK_Device='$Device'
	"
	CmdLine=$(RunSQL "$Q")

	if [[ -n "$CmdLine" ]]; then
		/usr/pluto/bin/"$CmdLine"
	fi
}

UpdatePorts()
{
	local Device="$1" Slot="$2"
	local Ports dev DevSlot
	Ports=()

	if [[ ! -d /sys/class/video4linux ]]; then
		return 0
	fi

	pushd /sys/class/video4linux >/dev/null
	for dev in *; do
		if [[ "$dev" != video* ]]; then
			continue
		fi
		DevSlot=$(readlink -f "$dev"/device)
		DevSlot="${DevSlot##*/}"
		DevSlot="${DevSlot#*:}"

		if [[ "$DevSlot" != "$Slot" ]]; then
			continue
		fi

		Ports=("${Ports[@]}" "$dev")
	done

	echo "Dev '$Device' Ports: ${Ports[*]}"
	# TODO: Add to database

	popd >/dev/null
}

CaptureCards=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Capture_Cards" "" "" "all")

for CardDevice in $CaptureCards; do
	Slot=$(CaptureCard_PCISlot "$CardDevice")
	if [[ -z "$Slot" ]]; then
		continue
	fi

	echo "--> Setting up card '$CardDevice'"

	ID="$(lshwd -id | grep "^$Slot" | cut -d' ' -sf2)"
	ID="${ID//:/}"
	if ! OurDevice "$CardDevice" "$Slot" "$ID"; then
		DisableDevice "$CardDevice"
	else
		EnableDevice "$CardDevice"
		UpdatePorts "$CardDevice" "$Slot"
	fi
done
