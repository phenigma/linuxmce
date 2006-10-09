#!/bin/bash

if [[ -n "$HEADER_Utils" ]]; then
	return 0
fi
HEADER_Utils=included

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

DEVICETEMPLATE_OnScreen_Orbiter=62
DEVICEDATA_Use_OpenGL_effects=172
DEVICEDATA_Use_alpha_blended_UI=169

FindDevice_Template()
{
	local PK_Device_Parent="${1//\'}" FK_DeviceTemplate="${2//\'}" NoRecursion="$3" IncludeParent="$4" All="$5"
	local Limit Found=0

	if [[ -z "$PK_Device_Parent" || -z "$FK_DeviceTemplate" ]]; then
		echo ""
		return 1
	fi

	if [[ -z "$All" ]]; then
		Limit="LIMIT 1"
	fi

	local i R Q
	if [[ "$PK_Device_Parent" == 0 ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			WHERE FK_Device_ControlledVia IS NULL AND FK_DeviceTemplate IN ($FK_DeviceTemplate)
			$Limit
		"
	elif [[ -z "$IncludeParent" ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceTemplate IN ($FK_DeviceTemplate)
			$Limit
		"
	else
		Q="
			SELECT PK_Device
			FROM Device
			WHERE (FK_Device_ControlledVia='$PK_Device_Parent' OR PK_Device='$PK_Device_Parent') AND FK_DeviceTemplate IN ($FK_DeviceTemplate))
			$Limit
		"
	fi
	R="$(RunSQL "$Q")"

	if [[ -n "$R" ]]; then
		echo "$R"
	fi

	if [[ ( -z "$R" || -n "$All" ) && -z "$NoRecursion" ]]; then
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent'"
		R="$(RunSQL "$Q")"
		for i in $R; do
			if FindDevice_Template "$i" "$FK_DeviceTemplate" "" "" "$All"; then
				if [[ -z "$All" ]]; then
					return 0
				else
					Found=1
				fi
			fi
		done
	else
		return 1
	fi

	[[ "$Found" -eq 1 ]]
	return $?
}

FindDevice_Category()
{
	local PK_Device_Parent="${1//\'}" FK_DeviceCategory="${2//\'}" NoRecursion="$3" IncludeParent="$4" All="$5"
	local Limit Found=0

	if [[ -z "$PK_Device_Parent" || -z "$FK_DeviceCategory" ]]; then
		echo ""
		return 1
	fi

	if [[ -z "$All" ]]; then
		Limit="LIMIT 1"
	fi

	local i R Q
	if [[ "$PK_Device_Parent" == 0 ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_Device_ControlledVia IS NULL AND FK_DeviceCategory IN ($FK_DeviceCategory)
			$Limit
		"
	elif [[ -z "$IncludeParent" ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceCategory IN ($FK_DeviceCategory)
			$Limit
		"
	else
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE (FK_Device_ControlledVia='$PK_Device_Parent' OR PK_Device='$PK_Device_Parent') AND FK_DeviceCategory IN ($FK_DeviceCategory)
			$Limit
		"
	fi
	R="$(RunSQL "$Q")"

	if [[ -n "$R" ]]; then
		echo "$R"
	fi

	if [[ ( -z "$R" || -n "$All" ) && -z "$NoRecursion" ]]; then
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent'"
		R="$(RunSQL "$Q")"
		for i in $R; do
			if FindDevice_Category "$i" "$FK_DeviceCategory" "" "" "$All"; then
				if [[ -z "$All" ]]; then
					return 0
				else
					Found=1
				fi
			fi
		done
	else
		return 1
	fi

	[[ "$Found" -eq 1 ]]
	return $?
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
	
#<-mkr_B_via_b->
	local VideoDriver=$(lshwd | grep ' VGA ' | sed 's/^.*(\([^()]*\)).*$/\1/')
	case "$VideoDriver" in
		nv) PackageIsInstalled nvidia-glx && VideoDriver="nvidia" ;;
		radeon|ati) PackageIsInstalled fglrx-driver && VideoDriver="fglrx" ;;
		"") VideoDriver="vesa" ;; # just-in-case default
	esac
#<-mkr_B_via_e->
#<-mkr_b_via_b->
	VideoDriver=viaprop
#<-mkr_b_via_e->
	echo "$VideoDriver"
}

ReloadDevicesOnThisMachine()
{
	for Dev in $(cat /usr/pluto/locks/pluto_spawned_local_devices.txt); do
		/usr/pluto/bin/MessageSend "$DCERouter" 0 "$Dev" 7 1
	done
}

AudioMixerVolume_Percent()
{
	local VolMin VolMax VolCur VolPercent
	local Settings

	Settings=$(amixer sget Master)
	
	VolMin=$(echo "$Settings" | grep -F 'Limits:' | sed 's/^  *//' | cut -d' ' -f3)
	VolMax=$(echo "$Settings" | grep -F 'Limits:' | sed 's/^  *//' | cut -d' ' -f5)
	VolCur=$(echo "$Settings" | grep -F 'Front Left:' | sed 's/^  *//' | cut -d' ' -f4)
	VolPercent=$((100 * $VolCur / $VolMax))

	echo "$VolPercent"
}

OpenGLeffects()
{
	local Q

	Q="
		SELECT IK_DeviceData
		FROM Device
		LEFT JOIN Device AS Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia
		JOIN Device_DeviceData ON Device.PK_Device=FK_Device
		WHERE
			Device.FK_DeviceTemplate='$DEVICETEMPLATE_OnScreen_Orbiter'
			AND (
				Device.FK_Device_ControlledVia='$PK_Device'
				OR Parent.FK_Device_ControlledVia='$PK_Device'
			)
		AND FK_DeviceData='$DEVICEDATA_Use_OpenGL_effects'
	"
	RunSQL "$Q"
}

AlphaBlendingEnabled()
{
	local Q

	Q="
		SELECT IK_DeviceData
		FROM Device
		LEFT JOIN Device AS Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia
		JOIN Device_DeviceData ON Device.PK_Device=FK_Device
		WHERE
			Device.FK_DeviceTemplate='$DEVICETEMPLATE_OnScreen_Orbiter'
			AND (
				Device.FK_Device_ControlledVia='$PK_Device'
				OR Parent.FK_Device_ControlledVia='$PK_Device'
			)
		AND FK_DeviceData='$DEVICEDATA_Use_alpha_blended_UI'
	"
	RunSQL "$Q"
}

UI_SetOptions()
{
	local OrbiterDev="$1"
	local OpenGLeffects="$2"
	local AlphaBlending="$3"
	local UI_Version="$4"

	# disable OpenGL effects
	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES('$OrbiterDev', '$DEVICEDATA_Use_OpenGL_effects', '$OpenGLeffects')
	"
	RunSQL "$Q"
	# disable alpha blending
	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES('$OrbiterDev', '$DEVICEDATA_Use_alpha_blended_UI', '$AlphaBlending')
	"
	RunSQL "$Q"
	# select UI1
	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES('$OrbiterDev', '$DEVICEDATA_PK_UI', '$UI_Version')
	"
	RunSQL "$Q"
}
