#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

DEVICEDATA_Block_Device=152

UpdatePorts_NoFind()
{
	local Device="$1" Port="$2"
	local Q

	if [[ -z "$Port" ]]; then
		return 1
	fi

	echo "$(date -R) Updating capture card BlockDevice DD; dev:$Device; port:$Port" | tee -a /var/log/pluto/CaptureCards_UpdatePorts.log
	Q="
		UPDATE Device_DeviceData
		SET IK_DeviceData='/dev/$Port'
		WHERE
			FK_DeviceData='$DEVICEDATA_Block_Device'
			AND FK_Device='$Device'
	"
	RunSQL "$Q"
}

# generic function for updating ports
# card scripts will probably want to override this
UpdatePorts()
{
	local Device="$1" Slot="$2"
	local Ports dev DevSlot Port
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

	Port=$(echo "${Ports[@]}" | sed 's/ /\n/g' | sort | head -1)
	echo "Dev '$Device' Ports: ${Ports[*]}; Chosen: $Port"
	UpdatePorts_NoFind "$Device" "$Port"

	popd >/dev/null
}
