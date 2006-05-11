#!/bin/bash

. /usr/pluto/bin/Utils.sh

BaseDir=/usr/pluto/bin

Wiz="$BaseDir"/AVWizard
AudioMixerSettingsAwk="$BaseDir"/AudioMixerSettings.awk

Resolutions_VESA=('640x480' '800x600' '1024x768' '1280x1024' '1600x1200')
Resolutions_HDTV=('480p=848x480' '720p=1280x720' '1080i=1920x1080i' '1080p=1920x1080')
Refresh_Rates=('50' '60' '65' '72' '75' '80' '85' '100' '120')

XF86Config="/etc/X11/xorg.conf.avwizard"

GetAudioMixerSettings()
{
	local Settings
	
	Settings="$(amixer sget Master | awk -f "$AudioMixerSettingsAwk")"
	eval "$Settings"
}

Resolution_GetFullName()
{
	local Ratio="$1" Prefix="$2"
	local Idx Resolution_Array FullName

	case "$Ratio" in
		4_3) Resolution_Array=("${Resolutions_VESA[@]}") ;;
		16_9) Resolution_Array=("${Resolutions_HDTV[@]}") ;;
	esac
	
	Idx=$(FindInArray_Prefix "$Prefix" "${Resolution_Array[@]}")
	if [[ -n "$Idx" ]]; then
		FullName="${Resolution_Array[$Idx]}"
	fi
	echo "$FullName"
}

Resolution_Get()
{
	local Ratio="$1" Resolution="$2" Op="$3"
	local Idx Resolution_Array

	case "$Ratio" in
		4_3) Resolution_Array=("${Resolutions_VESA[@]}") ;;
		16_9) Resolution_Array=("${Resolutions_HDTV[@]}") ;;
	esac
	
	Idx=$(FindInArray_Prefix "$Resolution" "${Resolution_Array[@]}")
	[[ -z "$Idx" ]] && Idx=0

	case "$Op" in
		+) [[ "$Idx" -lt "${#Resolution_Array[*]}" ]] && ((Idx++)) ;;
		-) [[ "$Idx" -gt 0 ]] && ((Idx--)) ;;
	esac
	echo "${Resolution_Array[$Idx]}"
}

Resolution_Next()
{
	local Ratio="$1" Resolution="$2"

	Resolution_Get "$Ratio" "$Resolution" +
}

Resolution_Prev()
{
	local Ratio="$1" Resolution="$2"

	Resolution_Get "$Ratio" "$Resolution" -
}

Refresh_Get()
{
	local Refresh="$1" Op="$2"
	local Idx

	Idx=$(FindInArray "$Refresh" "${Refresh_Rates[@]}")
	[[ -z "$Idx" ]] && Idx=0

	case "$Op" in
		+) [[ "$Idx" -lt "${#Refresh_Rates[@]}" ]] && ((Idx++)) ;;
		-) [[ "$Idx" -gt 0 ]] && ((Idx--)) ;;
	esac
	echo "${Refresh_Rates[$Idx]}"
}

Refresh_Next()
{
	local Refresh="$1"

	Refresh_Get "$Refresh" +
}

Refresh_Prev()
{
	local Refresh="$1"

	Refresh_Get "$Refresh" -
}

StartX()
{
	local Parm XParm
	XParm=()

	for Parm in "$@"; do
		XParm=("${XParm[@]}" -parm "$Parm")
	done
	"$BaseDir"/Start_X.sh -fg -client /usr/pluto/bin/AVWizard -srvparm '-config' -srvparm "$XF86Config" "${XParm[@]}"
}

StopX()
{
	"$BaseDir"/Stop_X.sh -client /usr/pluto/bin/AVWizard
}

RestartX()
{
	StopX
	StartX
}

WizSet()
{
	local Var="$1" Value="$2"
	
	[[ -z "$Var" ]] && return 1
	"$Wiz" -set "$Var" "$Value" 2>/dev/null
}

WizGet()
{
	local Var="$1"
	
	[[ -z "$Var" ]] && return 1
	"$Wiz" -get "$Var" 2>/dev/null
}
