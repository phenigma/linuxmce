#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Param="$1"
case "$Param" in
	volume[+-])
		GetAudioMixerSettings
		case "${Param#volume}" in
			+)
				if [[ "$AudioVolumeCurrent" -lt "$AudioVolumeMax" ]]; then
					((AudioVolumeCurrent++))
				fi
			;;
			-)
				if [[ "$AudioVolumeCurrent" -gt "$AudioVolumeMin" ]]; then
					((AudioVolumeCurrent--))
				fi
			;;
		esac
	;;
	-set) AudioVolumeCurrent="$2" ;;
esac
amixer sset Master "$AudioVolumeCurrent" unmute &>/dev/null
GetAudioMixerSettings
echo "$AudioVolumeCurrent"
