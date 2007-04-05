#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Q="SELECT FK_DeviceCategory FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device=$PK_Device"
DeviceCategory=$(RunSQL "$Q")

DeviceCategory_Core=7
DeviceCategory_MD=8

if [[ -n "$DeviceCategory" ]]; then
	if [[ $DeviceCategory -eq $DeviceCategory_MD ]]; then
#		if ! update-rc.d -f discover remove; then
#			:
#		fi
#		update-rc.d discover start 80 1 2 3 4 5 .
#
#		if ! update-rc.d -f hotplug remove; then
#			:
#		fi
#		update-rc.d hotplug start 81 1 2 3 4 5 . stop 89 0 6 . || /bin/true
		# Diskless machines should not drop portmap or networking at any point
		rm -f /etc/rc{0,6}.d/{S32portmap,S35networking}
	fi
fi

PrevVer="$2"

if [[ -n "$PrevVer" ]]; then
	echo "Upgrading from version '$PrevVer'. Not setting up X again"
else
	# only on standalone MDs, not hybrids
	if [ ! -e /etc/default/ntpdate.pbackup ] ;then
		cp /etc/default/ntpdate /etc/default/ntpdate.pbackup
	fi
	if ! PackageIsInstalled pluto-dcerouter; then
		sed -i 's/^NTPSERVERS=.*$/NTPSERVERS="dcerouter"/' /etc/default/ntpdate
	fi
fi
exit 0
