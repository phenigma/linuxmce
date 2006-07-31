#!/bin/bash

dmesg -n 1
printf '\x1b\x5b\x39\x3b\x30\x5d' >/dev/console  # disable console blanking (like 'setterm -blank' does, but we don't have it at this point)

# router/diskless
Type="router"

case "$Type" in
	router) Suffix=Core ;;
	diskless) Suffix=MD ;;
	*)
		echo "Invalid install type: '$Type'"
		while :; do sleep 1; done
	;;
esac
Cmd=/usr/pluto/install/Initial_Config_"$Suffix".sh

mkdir -p /var/log/pluto
File="/var/log/Pluto_Install.log"
: >"$File"
exec 3>&1 4>&2
bash -x "$Cmd" > >(tee -a "$File" >&3) 2> >(tee -a "$File" | grep -v '^++* ' >&4)
sleep 1 # so tee and grep close and flush their buffers
exec 3>&- 4>&-
exit 0
