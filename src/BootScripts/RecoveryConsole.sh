#!/bin/bash

exec </dev/tty11 >/dev/tty11

mke2fs -m 0 /dev/ram0
mkdir -p /mnt/recovery-rd
mount /dev/ram0 /mnt/recovery-rd
cd /mnt/recovery-rd
tar -xjf /usr/pluto/install/ramdisk.tar.bz2
screen -d -m -S MD_NetworkConnectivity /usr/pluto/bin/MD_NetworkConnectivity.sh
mount -t devfs devfs dev
while :; do
	echo "-- Diskless MD analysis and recovery console --"
	echo
	bin/sh
	bin/sleep 1
done
