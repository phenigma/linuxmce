#!/bin/bash

Kver=$(uname -r)
Kmaj=$(echo "$Kver" | cut -d. -f1)
Kmin=$(echo "$Kver" | cut -d. -f2)

if [ "$Kmaj.$Kmin" == "2.6" ]; then
	if [[ ! -d /home/coredump ]] ;then
		mkdir -p /home/coredump
	fi

	if [[ ! -s /usr/pluto/coredump ]] ;then
		ln -s /home/coredump /usr/pluto/coredump
	fi

	echo "/home/coredump/core_%e_%t_%s" >/proc/sys/kernel/core_pattern
elif [ "$Kmaj.$Kmin" == "2.4" ]; then
	echo 1 >/proc/sys/kernel/core_uses_pid
else
	echo "Not setting core settings. Unknown kernel version: '$Kmaj.$Kmin'"
fi
