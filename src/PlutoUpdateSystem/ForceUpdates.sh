#!/bin/bash

#MessageSend $IP -targetType template 0 26 1 67 13 "/usr/pluto/bin/ForceUpdates.sh"

## Kill all DownloadUpdates.sh scripts that are running
DownloadUpdates_PIDS=$(pidof DownloadUpdates.sh)
for pid in $DownloadUpdates_PIDS do;
	kill $pid
	wait $pid
done

## If onther ForceUpdates.sh instances are running, kill them too
ForceUpdates_PIDS=$(pidof ForceUpdates.sh)
for pid in $ForceUpdates_PIDS do;
	if [[ "$pid" != "$$" ]]; then
		kill $pid
		wait $pid
	fi
done

## Go ahead, force that upgrade
UpdatesOkStamp=$(date +%s)
touch /usr/pluto/var/Updates/UpdatesOk.flag
echo $UpdatesOkStamp > /usr/pluto/var/Updates/UpdatesOk.flag

## Force the reboot
reboot
