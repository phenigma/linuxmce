#!/bin/bash

while :; do
	/usr/pluto/bin/VoiceMailMonitor
	echo "VoiceMail Monitor exited with code '$?'"
	echo "---------------------------------------"
done | tee -a /var/log/pluto/VoiceMailMonitor.log
