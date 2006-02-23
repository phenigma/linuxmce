#!/bin/bash

#. /usr/pluto/bin/SQL_Ops.sh

# This file should be removed, until then is safe to put this here
exit 0

## Function to sync core's archive with MD's one
function syncMDwithCORE {
	# More faster to copy from here than to download
	cp -ru /var/cache/apt/archives-core /var/cache/apt/archives	
}

## Add some cron entries to download new debs
cronEntry="0 3 * * * root /usr/pluto/bin/Update_Packages.sh --download-only"
cronNotify="0 */10 * * * root /usr/pluto/bin/Update_Notify.sh"

if ! grep -qF "$cronEntry" /etc/crontab; then
	echo "$cronEntry" >>/etc/crontab
	/etc/init.d/cron reload
fi

## Are we called by cron or at startup ?
[[ -n "$1" ]] && DownloadOnly="Yes"
	
## Is this a diskless MD ?
IsDiskless=0
if [[ -f /etc/diskless.conf ]]; then
	IsDiskless=1
fi
	

## We are called by cron so we download only
if [[ "$DownloadOnly" == "Yes" ]]; then
	set -e
	
	echo "- Updating package lists"
	apt-get update 1>/dev/null

	if [[ $isDiskless -eq 1 ]]; then
		syncMDwithCORE
	fi
	
	echo "- Cleaning up dpkg journal"
	dpkg --forget-old-unavail

	echo "- Downloading updates"
	apt-get -f -y --download-only dist-upgrade 1>/dev/null

	touch /usr/pluto/locks/upgrade_ok

## We are called by a startup script so we need to do the update
else
	if [[ IsDiskless -eq 0 ]]; then
	#	LeftToDownload=$(apt-get dist-upgrade < /bin/false  | grep "Need to get" | cut -d" " -f4 | cut -d'/' -f1)
	#	if [[ -n "$LeftToDownload" &&  "$LeftToDownload" != "0B" ]]; then
	#		echo "- Need to get $LeftToDownload before doing the update"
	#		exit 0
	#	else
	#		echo "- No need to download any files"
	#	fi
		apt-get update
	else
		apt-get update
		syncMDwithCORE
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

