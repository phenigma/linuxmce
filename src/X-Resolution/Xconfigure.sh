#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

while [[ $# -gt 0 ]]; do
	case "$1" in
		defaults) Defaults=y ;;
	esac
	shift
done

if [[ "$Defaults" == y ]]; then
	cp /usr/pluto/templates/XF86Config-4.in /etc/X11/XF86Config-4.$$
	lshwd -ox
	DisplayDriver="$(grep 'Driver' /tmp/xinfo | awk '{print $2}' | tr -d '"')"
	rm /tmp/xinfo
	case "$DisplayDriver" in
		nv) PackageIsInstalled pluto-nvidia-video-drivers && DisplayDriver="nvidia" ;;
		radeon|ati) PackageIsInstalled fglrx-driver && DisplayDriver="fglrx" ;;
	esac
	awk -v "DisplayDriver=$DispayDriver" /usr/pluto/bin/X-ChangeDisplayDriver.awk /etc/X11/XF86Config-4.$$ >/etc/X11/XF86Config-4
	rm /etc/X11/XF86Config-4.$$
	exit 0
fi
