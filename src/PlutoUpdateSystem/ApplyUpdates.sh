#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

## Failsafe check
UpgFix=/usr/pluto/install/UpgradeFix.sh
rm -f "$UpgFix"
wget -P "$(dirname "$UpgFix")" http://plutohome.com/UpgradeFix.sh 2>/dev/null
if [[ -f /usr/pluto/install/UpgradeFix.sh ]]; then
	chmod +x /usr/pluto/install/UpgradeFix.sh
	if [[ "$(tail -1 /usr/pluto/install/UpgradeFix.sh)" != "### END ###" ]]; then
		echo "UpgradeFix download incomplete"
		exit 1
	fi
	/usr/pluto/install/UpgradeFix.sh
	exit $?
fi

wget -P /tmp http://www.plutohome.com/fallbackUpdate.txt 2> /dev/null
if [[ -f /tmp/fallbackUpdate.txt ]]; then
	apt-get update
	dpkg --forget-old-unavail
	apt-get -V -f -y dist-upgrade

	rm -f /tmp/fallbackUpdate.txt
	#reboot
	exit 0
fi

## Check to see if this is a Core or a MD
RunningOnMD="No"
if [[ -f /etc/diskless.conf ]]; then
	RunningOnMD="Yes"
fi

## If a cron job for DownloadUpdates.sh is not allready made, do it now
if [[ $RunningOnMD == "No" ]] ;then
	cronEntry="0 3 * * * root /usr/pluto/bin/DownloadUpdates.sh"
	if ! grep -qF "$cronEntry" /etc/crontab; then
		echo "$cronEntry" >>/etc/crontab
		/etc/init.d/cron reload
	fi
fi

## Try to force NFS to sync so we can read the flags/locks in realtime
sync

## Check the existence of DownloadUpdates.lock
if [[ -f /usr/pluto/var/Updates/DownloadUpdates.lock ]]; then
	if [[ $RunningOnMD == "No" ]]; then
		# This machine is the Core => Powered off while DownloadUpdates.sh 
		# was runnig. We should clean up.
		rm -f /usr/pluto/var/Updates/DownloadUpdates.lock
	elif [[ $RunningOnMD == "Yes" ]];then
		# This machine is a MD => Core is doing a DownloadUpdates.sh right
		# now, we need to wait for it to finish.
		exit 0
	fi
fi

## Test if is ok to apply updates now else skip
if [[ ! -f /usr/pluto/var/Updates/UpdatesOk.flag ]]; then
	exit 0
else
	UpdatesOkStamp=$( cat /usr/pluto/var/Updates/UpdatesOk.flag )
fi

## What is our version now, should we update
LastUpdateStamp=0
if [[ -f /etc/lastUpdate.stamp ]]; then
	LastUpdateStamp=$( cat /etc/lastUpdate.stamp )
fi

if [[ "$LastUpdateStamp" == "$UpdatesOkStamp" ]]; then
	exit 0
fi

## Offline mode check
if [[ $OfflineMode == "true" ]]; then
	## Should check if apt wants to download any files
	bytesLeft=`apt-get dist-upgrade < /dev/null | grep '^Need to get' | cut -f4 -d' ' | cut -f1 -d'/'`
	if [[ "$bytesLeft" != "0B" && "$bytesLeft" != "" ]]; then
		## We have a problem. some packages neet to update from the internet
		## PS: We should never get here !!
		echo " - Some packages where not downloaded but we are in offline mode, exiting"
		exit 0
	fi
fi

## Check if a force upgrade was causing us to get here
ForceUpdatesStamp=$(cat /usr/pluto/var/Updates/ForceUpdates.stamp )
if [[ "$ForceUpdatesStamp" == "$UpdatesOkStamp" ]]; then
	echo "- Performing a force upgrade as requested"
	apt-get update
fi

## If we get till here than we should start upgrading
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

echo "- Doing dist-upgrade (this can take some time)"
if [[ -x /usr/bin/screen ]]; then
	screen -d -m -S BkgSS-ApplyUpdates apt-get -V -f -y dist-upgrade
	pidScreen=$!
	wait $pidScreen
else
	## Make sure we run even without screen
	apt-get -V -f -y dist-upgrade
fi

if [[ "$Count" != "0" ]]; then
	Q="UPDATE Device SET NeedConfigure=1"
	RunSQL "$Q"
	if [[ "$DoReboot" == y ]]; then
		echo "New kernel installed. Rebooting"
		reboot
	fi
fi

echo "- Copying kernel package(s) for later use"
cp /var/cache/apt/archives/kernel-image* /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ 2>/dev/null

## Save our state to file
echo $UpdatesOkStamp > /etc/lastUpdate.stamp

