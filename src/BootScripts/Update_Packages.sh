#!/bin/bash

cronEntry="0 3 * * * root /usr/pluto/bin/Update_Packages.sh --download-only"

if ! grep -qF "$cronEntry" /etc/crontab; then
	echo "$cronEntry" >>/etc/crontab
	/etc/init.d/cron reload
fi

[[ -n "$1" ]] && DownloadOnly="--download-only"

mv /var/log/pluto/upgrade.newlog /var/log/pluto/upgrade.log 2>/dev/null
exec &> >(tee /var/log/pluto/upgrade.newlog)

echo "Performing system update"
apt-get update
apt-get -y $DownloadOnly dist-upgrade
cp /var/cache/apt/archives/kernel-image* /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ 2>/dev/null

[[ -z "$DownloadOnly" ]] && apt-get clean
