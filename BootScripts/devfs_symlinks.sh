#!/bin/bash

modprobe ide-generic

devfsd='# Pluto addition
REGISTER		^input/mice$	CFUNCTION GLOBAL mksymlink $devname mouse
UNREGISTER		^input/mice$	CFUNCTION GLOBAL unlink mouse
REGISTER		^cdroms/cdrom0$	CFUNCTION GLOBAL symlink $devname dvd
UNREGISTER		^cdroms/cdrom0$	CFUNCTION GLOBAL unlink dvd'
if ! grep -F '^#Pluto addition' /etc/devfs/devfsd.conf &>/dev/null; then
	echo "$devfsd" >>/etc/devfs/devfsd.conf
fi

/etc/init.d/devfsd reload
