#!/bin/bash

update-rc.d -f Startup_Scripts.sh remove || /bin/true
update-rc.d Startup_Scripts.sh defaults 22 10

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/PlutoVersion.h

# /etc/apt/apt.conf.d/30pluto

pluto_apt_conf='// Pluto apt conf add-on
//Acquire::http::Proxy "http://'"$DCERouter"':8123";
//Acquire::ftp::Proxy "ftp://'"$DCERouter"':8124";
//Acquire::ftp::ProxyLogin { "USER $(SITE_USER)@$(SITE):$(SITE_PORT)"; "PASS $(SITE_PASS)"; };
//Acquire::http::Proxy::dcerouter "DIRECT";
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
'

echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto

Q="SELECT FK_DeviceCategory FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device=$PK_Device"
DeviceCategory=$(RunSQL "$Q")

DeviceCategory_Core=7
DeviceCategory_MD=8

if [[ -n "$DeviceCategory" ]]; then
	if [[ $DeviceCategory -eq $DeviceCategory_MD ]]; then
		if ! update-rc.d -f discover remove; then
			:
		fi
		update-rc.d discover start 80 1 2 3 4 5 .

		if ! update-rc.d -f hotplug remove; then
			:
		fi
		update-rc.d hotplug start 81 1 2 3 4 5 . stop 89 0 6 . || /bin/true
	elif [[ $DeviceCategory -eq $DeviceCategory_Core ]]; then
		echo "$Version" >/home/pluto-version
	fi
fi
