#!/bin/bash

mkdir -p /etc/X11
/usr/pluto/bin/Xconfigure.sh --update-video-driver
for KVer in <-mkr_t_MakeRelease_Kernel->; do
	depmod "$KVer" || :
done
