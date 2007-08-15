#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh
. /usr/pluto/bin/Utils.sh

Param="$1"
case "$Param" in
	'Analog Stereo')
		XineConfSet audio.speaker_arrangement 'Stereo 2.0' "$XineConf"
		XineConfSet audio.alsa_front_device plughw:0 "$XineConf"
	;;
	'SPDIF Coaxial'|'SPDIF Optical')
		XineConfSet audio.speaker_arrangement 'Pass Through' "$XineConf"
		XineConfSet audio.alsa_front_device asym_spdif "$XineConf"
	;;
esac
/usr/pluto/bin/SetupAudioVideo.sh
