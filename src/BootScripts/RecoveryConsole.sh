#!/bin/bash

Connectivity()
{
	while ./bin/ping -c 1 dcerouter &>/dev/null; do
		./bin/sleep 5
	done
}

ShellLoop()
{
	exec </dev/tty11 &>/dev/tty11
	while :; do
		echo "-- Diskless MD analysis and recovery console --"
		echo
		bin/sh
		bin/sleep 1
	done
}

Emergency()
{
	Connectivity
	./bin/chvt-11
}

mke2fs -m 0 /dev/ram0
mkdir -p /mnt/recovery-rd
mount /dev/ram0 /mnt/recovery-rd
cd /mnt/recovery-rd
tar -xjf /usr/pluto/install/ramdisk.tar.bz2
mount -t devfs devfs dev
mount -t proc proc proc

Emergency &
ShellLoop &
