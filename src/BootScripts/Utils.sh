#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

FindDevice_Template()
{
	local PK_Device_Parent="${1//\'}" FK_DeviceTemplate="${2//\'}"

	if [[ -z "$PK_Device_Parent" || -z "$FK_DeviceTemplate" ]]; then
		echo ""
		return 1
	fi

	local i R Q
	Q="SELECT PK_Device
		FROM Device
		WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceTemplate='$FK_DeviceTemplate'"
	R="$(RunSQL "$Q")"

	if [[ -z "$R" ]]; then
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
	local PK_Device_Parent="${1//\'}" FK_DeviceCategory="${2//\'}"

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

	if [[ -z "$R" ]]; then
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
