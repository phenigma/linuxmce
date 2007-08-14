#!/bin/bash

AudioSettings="$1"

if [[ "$AudioSettings" == *[CO]* ]]; then
	# S/PDIF needs this to be set like this
	amixer sset 'IEC958' unmute >/dev/null
else
	# S/PDIF needs this to be set like this
	amixer sset 'IEC958' mute >/dev/null
fi
