#!/bin/bash

modprobe ide-generic

devfsd='
REGISTER		^input/mice$	CFUNCTION GLOBAL mksymlink $devname mouse
UNREGISTER		^input/mice$	CFUNCTION GLOBAL unlink mouse'
if ! grep -F 'mksymlink $devname mouse' /etc/devfs/devfsd.conf &>/dev/null; then
	echo "$devfsd" >>/etc/devfs/devfsd.conf
fi
