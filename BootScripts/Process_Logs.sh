#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

Filename="${PK_Device}_$(date +%F_%H-%M-%S)"
Dir="/var/log/pluto/critical"
Output="/var/log/pluto/archive"

mkdir -p "$Output"
rm -rf "$Dir"; mkdir -p "$Dir"

for i in /var/log/pluto/*.{,new}log; do
	grep "^01" "$i" >"$Dir/$(basename $i)"
done

tar -czf "$Output/$Filename.critical.tar.gz" "$Dir" /usr/pluto/coredump

rm -rf "$Dir"; mkdir -p "$Dir"

if [ "$1" != "0" ]; then
	ftp-upload --ignore-quit-failure -h plutohome.com --passive -b -d upload "$Output/$Filename.critical.tar.gz" &
fi

tar -czf "$Output/log-$Filename.tar.gz" /var/log/pluto/*.{,new}log
rm -f /var/log/pluto/*.{,new}log

