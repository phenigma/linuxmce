#!/bin/bash

. /usr/pluto/bin/Utils.sh

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

mkdir -p /etc/X11
bash -x /usr/pluto/bin/Xconfigure.sh --update-video-driver --keep-resolution | tee-pluto /var/log/pluto/Xconfigure.log
#rm -f /etc/modprobe.d/lrm-video || :
if ! BlacklistConfFiles '/etc/X11/xorg.conf' ;then
	if [ ! -e /etc/X11/xorg.conf.pbackup ] ;then
		cp /etc/X11/xorg.conf /etc/X11/xorg.conf.pbackup || :
	fi
	sed -i 's/"nv"/"vesa"/g' /etc/X11/xorg.conf || :
	sed -i 's/"nvidia"/"vesa"/g' /etc/X11/xorg.conf || :
fi 
for KVer in <-mkr_t_MakeRelease_Kernel->; do
	depmod "$KVer" || :
done
