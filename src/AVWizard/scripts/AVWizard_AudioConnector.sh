#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh
. /usr/pluto/bin/Utils.sh

Param="$1"
case "$Param" in
	'Analog Stereo')
		XineConfSet audio.speaker_arrangement 'Stereo 2.0' "$XineConf"
		XineConfSet audio.alsa_front_device default "$XineConf"
		XineConfSet audio.a52_pass_through 0 "$XineConf"
		XineConfSet audio.five_lfe_channel 0 "$XineConf"
	;;
	'SPDIF Coaxial'|'SPDIF Optical')
		XineConfSet audio.speaker_arrangement 'Pass Through' "$XineConf"
		XineConfSet audio.alsa_front_device spdif "$XineConf"
		XineConfSet audio.a52_pass_through 1 "$XineConf"
		XineConfSet audio.five_lfe_channel 1 "$XineConf"
		amixer sset 'IEC958 Playback Source' 'AC-Link' >/dev/null
	;;
esac
