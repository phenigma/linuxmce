#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

if [[ ! -e /etc/ld.so.conf.d/plutolibs ]] ;then 
	echo "/usr/pluto/lib" >>/etc/ld.so.conf.d/plutolibs
	ldconfig
fi
