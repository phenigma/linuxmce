#!/bin/bash

dmesg -n 1

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
File="/var/log/pluto/Initial_Install.log"
: >"$File"
exec 3>&1 4>&2
bash -x "$Cmd" > >(tee -a "$File" >&3) 2> >(tee -a "$File" | grep -v '^++* ' >&4)
sleep 1 # so tee and grep close and flush their buffers
exec 3>&1 4>&2
exit 0
