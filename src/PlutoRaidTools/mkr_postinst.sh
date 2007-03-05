#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

result=$(cat /etc/mdadm/mdadm.conf | grep 'PROGRAM /usr/pluto/bin/monitoring_RAID.sh' || /bin/true)
if [[ -z $result ]] ;then
	echo "PROGRAM /usr/pluto/bin/monitoring_RAID.sh" >> /etc/mdadm/mdadm.conf
	invoke-rc.d mdadm reload
fi
