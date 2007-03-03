#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0"

mkdir -p /etc/X11
bash -x /usr/pluto/bin/Xconfigure.sh --update-video-driver --keep-resolution | tee /var/log/pluto/Xconfigure.log
