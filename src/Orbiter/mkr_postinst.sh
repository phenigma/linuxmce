#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

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
	if ! BlacklistConfFiles '/etc/default/ntpdate' ;then
		if [ ! -e /etc/default/ntpdate.pbackup ] && [ -e /etc/default/ntpdate ] ;then
			cp /etc/default/ntpdate /etc/default/ntpdate.pbackup || :
		fi
		if ! PackageIsInstalled pluto-dcerouter; then
			sed -i 's/^NTPSERVERS=.*$/NTPSERVERS="dcerouter"/' /etc/default/ntpdate
		fi
	fi
fi

if [ -e /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc ] ;then
	flag='0'
	if ! grep -qF 'excludeApps=xfwm4' /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc ;then
			while read line ;do
					if [[ "$line" == "[General]" ]] ;then
							flag='1'
					fi
					if [[ "$flag" == "1" ]] ;then
							echo "$line" >> /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc.$$
							echo "excludeApps=xfwm4" >> /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc.$$
							flag='0'
					else
							echo "$line" >> /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc.$$
					fi
			done < /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc
			mv /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc.$$ /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc
	fi
fi

#<-mkr_b_via_b->
bash -x /home/via/ApplyViaPatch.sh
#<-mkr_b_via_e->
exit 0
