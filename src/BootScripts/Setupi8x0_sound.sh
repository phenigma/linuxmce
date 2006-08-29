#!/bin/bash

AudioSettings="$1"

if [[ "$AudioSettings" == *[CO]* ]]; then
	# S/PDIF needs this to be set like this
	amixer sset 'IEC958 Playback Source' 'AC-Link' >/dev/null
fi
# everything else works out of the box
