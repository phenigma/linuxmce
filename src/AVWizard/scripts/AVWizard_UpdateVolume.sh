#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Param="$1"
case "$Param" in
	volume[+-])
		GetAudioMixerSettings
		case "${Param#volume}" in
			+)
				/usr/pluto/bin/SoundCards_AudioVolume.sh up
			;;
			-)
				/usr/pluto/bin/SoundCards_AudioVolume.sh down
			;;
		esac
	;;
	-set)
		/usr/pluto/bin/SoundCards_AudioVolume.sh set "$2"
	;;
esac
GetAudioMixerSettings
echo "$AudioVolumeCurrent"
