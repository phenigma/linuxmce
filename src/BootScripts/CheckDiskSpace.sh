#!/bin/bash

. /usr/pluto/bin/pluto.func

CoreThreshold=50 # per Prefix
LogSizeThreshold=100 # MB # TODO: remove and reset open file descriptors [without breaking anything?]
LogKeepLines=100 # number of lines to keep
UsageThreshold=95 # %

cronEntry="0 */1 * * * root /usr/pluto/bin/CheckDiskSpace.sh"

LineField()
{
	echo "$1" | awk "{print \$$2}"
}

if ! grep -qF '/usr/pluto/bin/CheckDiskSpace.sh' /etc/crontab; then
	echo "$cronEntry" >>/etc/crontab
	/etc/init.d/cron reload
fi

df -h / /home | tail +2 | while read line; do
	Usage=$(LineField "$line" 5)
	Mount=$(LineField "$line" 6)

	Usage=${Usage%\%}

	if [[ "$Mount" == "/home" && "$Usage" -ge "$UsageThreshold" ]]; then
		Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Disk usage ($Usage%) above threshold ($UsageThreshold%) in /home"
		# TODO: Send a one-time message to all orbiters
	fi

	if [[ "$Mount" == "/" && "$Usage" -ge "$UsageThreshold" ]]; then
		Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Disk usage ($Usage%) above threshold ($UsageThreshold%) in /"
		
		pushd /usr/pluto/coredump >/dev/null
		Prefix=""
		Count=0
		(ls -1; echo) | while read file; do
			file=${file#core_}
			NewPrefix="${file%%_*}"
			if [[ "$NewPrefix" == "$Prefix" && -n "$NewPrefix" ]]; then
				((Count++))
			else
				if [[ "$Count" -ge "$CoreThreshold" ]]; then
					Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Core dump count ($Prefix: $Count) above threshold ($CoreThreshold)"
					rm -f core_${Prefix}_*
				fi
				Count=1
				Prefix="$NewPrefix"
			fi
		done
		popd >/dev/null
	fi
done
