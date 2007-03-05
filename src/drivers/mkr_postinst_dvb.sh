#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

if ! grep -q 'dvb[-_]bt8xx' /etc/modules; then
	echo "dvb-bt8xx" >>/etc/modules
	modprobe dvb-bt8xx || :
fi
