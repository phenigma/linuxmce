#!/bin/bash

while :; do
	/usr/pluto/bin/VoiceMailMonitor
	echo "VoiceMail Monitor exited with code '$?'"
	echo "---------------------------------------"
done | tee-pluto -a /var/log/pluto/VoiceMailMonitor.log
