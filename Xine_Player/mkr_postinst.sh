#!/bin/bash

NoSound()
{
	[ "$1" == "0" ] && return 0
	echo "It seems you don't have a (auto-configurable) sound card. You won't be able to play any sound."
	return 0
}

# I can't think of another way to load the sound card drivers
/etc/init.d/hotplug start

amixer sset Master 74% unmute || NoSound
amixer sset PCM 74% unmute || NoSound 0
