#!/bin/bash

Kver=$(uname -r)
Kmaj=$(echo "$Kver" | cut -d. -f1)
Kmin=$(echo "$Kver" | cut -d. -f2)

if [ "$Kmaj.$Kmin" == "2.6" ]; then
	mkdir -p /usr/pluto/coredump
	echo "/usr/pluto/coredump/core_%e_%t_%s" >/proc/sys/kernel/core_pattern
elif [ "$Kmaj.$Kmin" == "2.4" ]; then
	echo 1 >/proc/sys/kernel/core_uses_pid
else
	echo "Not setting core settings. Unknown kernel version: '$Kmaj.$Kmin'"
fi
