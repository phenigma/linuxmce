#!/bin/bash

#if [[ "$1" != "background" ]] ;then
#	creen -d -m -S Upgrade_Download "$0" background
#	exit 0
#fi

mkdir -p /var/gutsy-upgrade-scripts/ 

DOWNLOAD_PID_FILE="/var/gutsy-upgrade-scripts/download.pid"
DOWNLOAD_PID=$(cat "$DOWNLOAD_PID_FILE")

if [[ "$DOWNLOAD_PID" != "" ]] ;then
	if [[ ! -d "/proc/$DOWNLOAD_PID" ]] ;then
		echo "$$" > "$DOWNLOAD_PID_FILE"
	else
		exit 1
	fi
else
	echo "$$" > "$DOWNLOAD_PID_FILE"
fi

wget --directory-prefix=/var/gutsy-upgrade-scripts/ --continue --tries=5 --timeout=5  --output-file=/var/gutsy-upgrade-scripts/download.log \
	ftp://ftp.iasi.roedu.net/mirrors/ubuntulinux.org/releases/7.10/ubuntu-7.10-alternate-amd64.iso || exit 1


