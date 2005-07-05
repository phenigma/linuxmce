#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

SetSound_i8x0()
{
	local SoundSetting="$1"

	case "$SoundSetting" in
		# Manual
		M) ;;

		# Coax
		C) amixer sset 'IEC958 Playback Source' 'AC-Link' ;;

		# TosLink
		O) amixer sset 'IEC958 Playback Source' 'A/D Converter' ;;

		# Stereo
		S) ;;

		# Multi-channel analog
		L) ;;
	esac
}

SetSound_Audigy()
{
	local SoundSetting="$1"

	case "$SoundSetting" in
		# Manual
		M) ;;

		# Coax
		C) ;;

		# TosLink
		O) ;;

		# Stereo
		S) ;;

		# Multi-channel analog
		L) ;;
	esac
}

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$PK_Device AND FK_DeviceData=88"
R="$(RunSQL "$Q")"

if [[ -z "$R" ]]; then
	Q="SELECT IK_DeviceData,PK_Device FROM Device_DeviceData JOIN Device ON FK_Device=PK_Device WHERE FK_Device_ControlledVia=$PK_Device AND FK_DeviceData=88 AND FK_DeviceTemplate=28"
	R="$(RunSQL "$Q")"
	PK_Device="$(Field 2 "$R")"
	R="$(Field 1 "$R")"
fi

if [[ -z "$R" ]]; then
	echo "No setting"
	exit
fi

SoundSetting="$R"

# 1621 - i8x0; 1607 - Audigy
DT="1621,1607"
Q="SELECT FK_DeviceTemplate FROM Device WHERE FK_DeviceTemplate IN ($DT) AND FK_Device_ControlledVia=$PK_Device"
R="$(RunSQL "$Q")"

if [[ -z "$R" ]]; then
	echo "No (supported) card"
	exit
fi

Type="$R"

case "$Type" in
	# i8x0
	1621)
		SetSound_i8x0 "$SoundSetting"
	;;

	# Audigy
	1607)
		SetSound_Audigy "$SoundSetting"
	;;
esac
