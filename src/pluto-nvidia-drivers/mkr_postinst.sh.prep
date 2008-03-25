#!/bin/bash

. /usr/pluto/bin/Utils.sh

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

mkdir -p /etc/X11
bash -x /usr/pluto/bin/Xconfigure.sh --update-video-driver --keep-resolution | tee-pluto /var/log/pluto/Xconfigure.log
rm -f /etc/modprobe.d/lrm-video || :
#for KVer in <-mkr_t_MakeRelease_Kernel->; do
#	depmod "$KVer" || :
#done

#mkdir -p /etc/rcS.d || :
#echo "#!/bin/bash
#modprobe -r nvidia
#modprobe nvidia
#" > /etc/rcS.d/99_fix_nvidia_kernel  || :
#chmod +x /etc/rcS.d/99_fix_nvidia_kernel || :
