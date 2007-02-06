#!/bin/bash

mkdir -p /etc/X11
bash -x /usr/pluto/bin/Xconfigure.sh --update-video-driver --keep-resolution | tee-pluto /var/log/pluto/Xconfigure.log
for KVer in <-mkr_t_MakeRelease_Kernel->; do
	depmod "$KVer" || :
done
