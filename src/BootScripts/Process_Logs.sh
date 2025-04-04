#!/bin/bash

# Offlined until further decisions are made, although it seems to be disabled already
exit 0

. /usr/pluto/bin/Config_Ops.sh

Param="$1"

Filename="${PK_Installation}_${PK_Device}_$(date +%F_%H-%M-%S)"
Critical="/var/log/pluto/critical"
Output="/var/log/pluto/archive"

UploadLogs()
{
	local ok

	ok=0
	
	if [[ "$OfflineMode" == "true" ]]; then
		echo "$(date) Offline mode, can't upload any files." >> /var/log/pluto/ftp-upload.log
		ok=1
	fi

	while [ "$ok" -eq 0 ]; do
		echo "$(date) Uploading critical errors" >>/var/log/pluto/ftp-upload.log
		ftp-upload -v --ignore-quit-failure -h plutohome.com --passive -b -d incoming "$Output/$Filename.critical.tar.gz" >>/var/log/pluto/ftp-upload.log 2>>/var/log/pluto/ftp-upload-err.log
		if [ "$?" -eq 0 ]; then
			echo "$(date) Done" >>/var/log/pluto/ftp-upload.log
			ok=1
		else
			echo "$(date) Failed. Retrying" >>/var/log/pluto/ftp-upload.log
			sleep 3600
		fi
	done
}

ArchiveCoreDumps()
{
	nice -n 20 tar -czf "$Output/$Filename.critical.tar.gz" "$Critical" /usr/pluto/coredump/archive 2>/dev/null
	rm -rf /usr/pluto/coredump/archive
	rm -rf "$Critical"

	/usr/pluto/bin/MonitorDiskSpace.sh 
	if [ "$1" != "0" ]; then
		UploadLogs &
	fi
}

ArchiveLogs()
{
	nice -n 20 tar -czf "$Output/log-$Filename.tar.gz" /var/log/pluto/log.archive/ 2>/dev/null
	rm -rf /var/log/pluto/log.archive

	find /var/log/pluto/ -mtime +5 -exec rm -f '{}' ';'
}

mkdir -p "$Output"
mkdir -p "$Critical"

for i in /var/log/pluto/*.{,new}log; do
	grep "^01" "$i" >"$Critical/$(basename $i)"
done

mkdir -p /usr/pluto/coredump/archive
CoreFiles=$(echo /usr/pluto/coredump/*)
[ -n "$CoreFiles" ] && mv /usr/pluto/coredump/* /usr/pluto/coredump/archive/
ArchiveCoreDumps "$Param" &

mkdir -p /var/log/pluto/log.archive
mv /var/log/pluto/*.log /var/log/pluto/log.archive
cp /var/log/pluto/*.newlog /var/log/pluto/log.archive
ArchiveLogs &
