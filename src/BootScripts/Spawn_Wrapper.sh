#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

set -x

DEVICEDATA_Audio_Settings=88
DEVICECATEGORY_Media_Director=8
DEVICEDATA_Output_Speaker_arrangement=71

UpdateXineConf()
{
	Q="
		SELECT FK_DeviceCategory
		FROM Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE PK_Device='$PK_Device'
	"
	Category=$(RunSQL "$Q")
	
	if [[ -z "$Category" ]]; then
		return 1
	elif [[ "$Category" != "$DEVICECATEGORY_Media_Director" ]]; then
		# it's the Core; search for its embedded MD
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			WHERE FK_Device_ControlledVia='$PK_Device' AND PK_DeviceCategory='$DEVICECATEGORY_Media_Director'
		"
		MD=$(RunSQL "$Q")
		if [[ -z "$MD" ]]; then
			# No embedded MD found
			return 1
		fi
	else
		MD="$PK_Device"
	fi

	Q="
		SELECT IK_DeviceData
		FROM Device_DeviceData
		WHERE FK_Device='$MD' AND FK_DeviceData='$DEVICEDATA_Audio_Settings'
	"
	AudioSetting=$(RunSQL "$Q")

	if [[ "$AudioSetting" == *3* ]]; then
		XineConfSet audio.speaker_arrangement "Pass Through"
		XineConfSet audio.alsa_front_device spdif
		amixer sset 'IEC958 Playback Source' 'AC-Link' >/dev/null

		Q="
			UPDATE Device_DeviceData
			SET IK_DeviceData='Pass Through'
			WHERE FK_Device='$SpawnedDevice' AND FK_DeviceData='$DEVICEDATA_Output_Speaker_arrangement'
		"
		RunSQL "$Q"
	else
		XineConfSet audio.speaker_arrangement "Stereo 2.0"
		XineConfSet audio.alsa_front_device default

		Q="
			UPDATE Device_DeviceData
			SET IK_DeviceData='Stereo 2.0'
			WHERE FK_Device='$SpawnedDevice' AND FK_DeviceData='$DEVICEDATA_Output_Speaker_arrangement'
		"
		RunSQL "$Q"
	fi

	if [[ "$AudioSetting" == *K* ]]; then
		XineConfSet audio.synchronization.force_rate 48000
	else
		XineConfSet audio.synchronization.force_rate 0
	fi
}

for ((i = 2; i <= "$#"; i++)); do
	case "${!i}" in
		-d) ((i++)); SpawnedDevice=${!i} ;;
	esac
done

Spawnee="$(basename "$1")"
case "$Spawnee" in
	Xine_Player) UpdateXineConf ;;
esac

PID=$$
cmd_line="$1"
echo "$PID $cmd_line (by $0)" >>/var/log/pluto/running.pids
exec "$@"
