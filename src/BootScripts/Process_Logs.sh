#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

Param="$1"

UploadLogs()
{
	local ok

	ok=0
	while [ "$ok" -eq 0 ]; do
		echo "$(date) Uploading critical errors" >>/var/log/pluto/ftp-upload.log
		ftp-upload -v --ignore-quit-failure -h plutohome.com --passive -b -d incoming "$Output/$Filename.critical.tar.gz" >>/var/log/pluto/ftp-upload.log 2>>/var/log/pluto/ftp-upload-err.log
		if [ "$?" -eq 0 ]; then
			echo "$(date) Done" >>/var/log/pluto/ftp-upload.log
		else
			echo "$(date) Failed. Retrying" >>/var/log/pluto/ftp-upload.log
			sleep 3600
		fi
	done
}

ArchiveCoreDumps()
{
	tar -czf "$Output/$Filename.tar.gz" "$Critical" /usr/pluto/coredump.archive 2>/dev/null
	rm -rf /usr/pluto/coredump.archive
	rm -rf "$Critical"

	if [ "$1" != "0" ]; then
		UploadLogs &
	fi
}

Filename="${PK_Device}_$(date +%F_%H-%M-%S)"
Critical="/var/log/pluto/critical"
Output="/var/log/pluto/archive"

mkdir -p "$Output"
mkdir -p "$Critical"

for i in /var/log/pluto/*.{,new}log; do
	grep "^01" "$i" >"$Critical/$(basename $i)"
done

mkdir -p /usr/pluto/coredump.archive
CoreFiles=$(echo /usr/pluto/coredump/*)
[ -n "$CoreFiles" ] && mv /usr/pluto/coredump/* /usr/pluto/coredump.archive/
ArchiveCoreDumps "$Param" &

tar -czf "$Output/log-$Filename.tar.gz" /var/log/pluto/*.{,new}log 2>/dev/null
rm -f /var/log/pluto/*.log

find "$Output" -mtime +5 -exec rm -f '{}' ';'
