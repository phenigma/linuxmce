#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

FindDevice_Template()
{
	local PK_Device_Parent="${1//\'}" FK_DeviceTemplate="${2//\'}" NoRecursion="$3" IncludeParent="$4"

	if [[ -z "$PK_Device_Parent" || -z "$FK_DeviceTemplate" ]]; then
		echo ""
		return 1
	fi

	local i R Q
	if [[ "$PK_Device_Parent" == 0 ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			WHERE FK_Device_ControlledVia IS NULL AND FK_DeviceTemplate IN ($FK_DeviceTemplate)
		"
	elif [[ -z "$IncludeParent" ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceTemplate IN ($FK_DeviceTemplate)
		"
	else
		Q="
			SELECT PK_Device
			FROM Device
			WHERE (FK_Device_ControlledVia='$PK_Device_Parent' OR PK_Device='$PK_Device_Parent') AND FK_DeviceTemplate IN ($FK_DeviceTemplate))
		"
	fi
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
	local PK_Device_Parent="${1//\'}" FK_DeviceCategory="${2//\'}" NoRecursion="$3" IncludeParent="$4"

	if [[ -z "$PK_Device_Parent" || -z "$FK_DeviceCategory" ]]; then
		echo ""
		return 1
	fi

	local i R Q
	if [[ "$PK_Device_Parent" == 0 ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_Device_ControlledVia IS NULL AND FK_DeviceCategory IN ($FK_DeviceCategory)
		"
	elif [[ -z "$IncludeParent" ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceCategory IN ($FK_DeviceCategory)
		"
	else
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE (FK_Device_ControlledVia='$PK_Device_Parent' OR PK_Device='$PK_Device_Parent') AND FK_DeviceCategory IN ($FK_DeviceCategory)
		"
	fi
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
	local XineConf="${3:-/etc/pluto/xine.conf}"
	
	[[ -z "$Setting" || -z "$Value" ]] && return 1
	
	if grep -qF "$Setting" "$XineConf"; then
		sed -i "s/^.*$Setting:.*$/$Setting:$Value/" "$XineConf"
	else
		echo "$Setting:$Value" >>"$XineConf"
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

FindInArray()
{
	local Value="$1" Array i
	shift
	Array=("$@")
	
	for ((i = 0; i < ${#Array[*]}; i++)); do
		if [[ "${Array[$i]}" == "$Value" ]]; then
			echo "$i"
			return 0
		fi
	done
	return 1
}

FindInArray_Prefix()
{
	local Value="$1" Array i
	shift
	Array=("$@")

	for ((i = 0; i < ${#Array[*]}; i++)); do
		if [[ "${Array[$i]}" == "$Value"* ]]; then
			echo "$i"
			return 0
		fi
	done
	return 1
}

GetVideoDriver()
{
	if [[ -n "$ForceVESA" ]]; then
		echo vesa
		return 0
	fi
	
	lshwd -ox >/dev/null
	local VideoDriver="$(grep 'Driver' /tmp/xinfo | awk -F'"' '{print $2}')"
	rm /tmp/xinfo
	case "$VideoDriver" in
		nv) PackageIsInstalled nvidia-glx && VideoDriver="nvidia" ;;
		radeon|ati) PackageIsInstalled fglrx-driver && VideoDriver="fglrx" ;;
		"") VideoDrivers="vesa" ;; # just-in-case default
	esac
	echo "$VideoDriver"
}
