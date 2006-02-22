#!/bin/bash

#. /usr/pluto/bin/SQL_Ops.sh

cronEntry="0 3 * * * root /usr/pluto/bin/Update_Packages.sh --download-only"
cronNotify="0 */10 * * * root /usr/pluto/bin/Update_Notify.sh"

if ! grep -qF "$cronEntry" /etc/crontab; then
	echo "$cronEntry" >>/etc/crontab
	/etc/init.d/cron reload
fi

[[ -n "$1" ]] && DownloadOnly="Yes"


if [[ "$DownloadOnly" == "Yes" ]]; then
	set -e
	
	echo "- Updating package lists"
	apt-get update 1>/dev/null

	echo "- Cleaning up dpkg journal"
	dpkg --forget-old-unavail

	echo "- Downloading updates"
	apt-get -f -y --download-only dist-upgrade 1>/dev/null
	touch /usr/pluto/locks/upgrade_ok
else
	IsDiskless=0
	if [[ -f /etc/diskless.conf ]]; then
		IsDiskless=1
	fi
	
	if [[ IsDiskless -eq 0 ]]; then
		LeftToDownload=$(apt-get dist-upgrade < /bin/false  | grep "Need to get" | cut -d" " -f4 | cut -d'/' -f1)
		if [[ -n "$LeftToDownload" &&  "$LeftToDownload" != "0B" ]]; then
			echo "- Need to get $LeftToDownload before doing the update"
			exit 0
		else
			echo "- No need to download any files"
		fi
	fi
		

	echo "- Previewing dist-upgrade"
	InstPkgs="$(apt-get -s -f dist-upgrade | grep "^Conf " | cut -d' ' -f2 | tr '\n' ' ')"
	RebootPkg="pluto-kernel-upgrade"
	for Pkg in $RebootPkg; do
		if [[ "$InstPkgs" == *"$Pkg"* ]]; then
			echo "- A reboot will be performed after the updates are applied"
			DoReboot=y
			break
		fi
	done

	Count=$(apt-get -f -y -s dist-upgrade | egrep -c '^Inst |^Conf ')

	echo "- Doing dist-upgrade"
	if apt-get -V -f -y dist-upgrade; then
		if [[ "$Count" != "0" ]]; then
			Q="UPDATE Device SET NeedConfigure=1"
			RunSQL "$Q"
			if [[ "$DoReboot" == y ]]; then
				echo "New kernel installed. Rebooting"
				reboot
			fi
		fi
	fi

	echo "- Copying kernel package(s) for later use"
	cp /var/cache/apt/archives/kernel-image* /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ 2>/dev/null
fi

