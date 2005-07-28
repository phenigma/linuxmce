#!/bin/bash

cronEntry="0 3 * * * root /usr/pluto/bin/Update_Packages.sh --download-only"
cronNotify="0 */10 * * * root /usr/pluto/bin/Update_Notify.sh"

Lock=/usr/pluto/locks/upgrade_ok
rm -f "$Lock"

if ! grep -qF "$cronEntry" /etc/crontab; then
	echo "$cronEntry" >>/etc/crontab
	/etc/init.d/cron reload
fi

[[ -n "$1" ]] && DownloadOnly="--download-only"

mv /var/log/pluto/upgrade.newlog /var/log/pluto/upgrade.log 2>/dev/null
exec &> >(tee /var/log/pluto/upgrade.newlog)

echo "Performing system update"
apt-get update
Fail=0
if apt-get -f -y $DownloadOnly dist-upgrade && [[ -n "$DownloadOnly" ]]; then
	date -R >"$Lock"
else
	Fail=1
	cp /var/log/pluto/upgrade.newlog /usr/pluto/coredump/
fi

cp /var/cache/apt/archives/kernel-image* /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ 2>/dev/null

if [[ -z "$DownloadOnly" && $Fail -eq 0 ]]; then
	apt-get clean
fi
