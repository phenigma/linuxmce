#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

FindDevice_Template()
{
	local PK_Device_Parent="${1//\'}" FK_DeviceTemplate="${2//\'}" NoRecursion="$3"

	if [[ -z "$PK_Device_Parent" || -z "$FK_DeviceTemplate" ]]; then
		echo ""
		return 1
	fi

	local i R Q
	Q="SELECT PK_Device
		FROM Device
		WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceTemplate='$FK_DeviceTemplate'"
	R="$(RunSQL "$Q")"

	if [[ -z "$R" && -z "$NoRecusion" ]]; then
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent'"
		R="$(RunSQL "$Q")"
		for i in $R; do
			FindDevice_Template "$i" "$FK_DeviceTemplate" && return 0
		done
	else
		echo "$R"
		return 0
	fi
	return 1
}

FindDevice_Category()
{
	local PK_Device_Parent="${1//\'}" FK_DeviceCategory="${2//\'}" NoRecursion="$3"

	if [[ -z "$PK_Device_Parent" || -z "$FK_DeviceCategory" ]]; then
		echo ""
		return 1
	fi

	local i R Q
	Q="SELECT PK_Device
		FROM Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceCategory='$FK_DeviceCategory'"
	R="$(RunSQL "$Q")"

	if [[ -z "$R" && -z "$NoRecursion" ]]; then
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent'"
		R="$(RunSQL "$Q")"
		for i in $R; do
			FindDevice_Category "$i" "$FK_DeviceCategory" && return 0
		done
	else
		echo "$R"
		return 0
	fi
	return 1
}

XineConfSet()
{
	local Setting="$1"
	local Value="$2"
	
	[[ -z "$Setting" || -z "$Value" ]] && return 1
	
	if grep -qF "$Setting" /etc/pluto/xine.conf; then
		sed -i "s/^.*$Setting:.*$/$Setting:$Value/" /etc/pluto/xine.conf
	else
		echo "$Setting:$Value" >>/etc/pluto/xine.conf
	fi
}

DeviceIsDisabled()
{
	local PK_Device="$1"
	local R Q

	[[ -z "$PK_Device" ]] && return 1

	Q="SELECT COUNT(*) FROM Device WHERE PK_Device='$PK_Device' AND Disabled=0"
	R=$(RunSQL "$Q")

	if [[ -n "$R" && "$R" -eq 0 ]]; then
		return 0 # Device is disabled or does not exist at all
	else
		return 1 # Device exists and is enabled
	fi
}
