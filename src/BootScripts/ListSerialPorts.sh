#!/bin/bash

# TODO: USB-to-Serial (Debian removed Keyspan firmware from the kernel sources because it violates DFSG)
Ports=$(cat /proc/tty/driver/serial | grep -v unknown | grep ': ' | cut -d: -f1)
for i in $Ports; do
	echo "/dev/ttyS$i"
done
