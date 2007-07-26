#!/bin/bash

while :; do
	/usr/pluto/bin/VoiceMailMonitor
	echo "VoiceMail Monitor exited with code '$?'"
	echo "---------------------------------------"
done 2>&1 | tee-pluto -a /var/log/pluto/VoiceMailMonitor.log
