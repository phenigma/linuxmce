#!/bin/bash

echo "*CRITICAL ERRORS OCCURED*" >> /critical.log
echo "=========================" >> /critical.log
echo "sumarized errors" >> /tmp/status.log

for logFile in /var/log/pluto/* ;do
	file=$(basename "$logFile")
	if [[ "$file" == [0-9]?*log ]] || [[ "$file" == "DCERouter.log" ]] ;then
		cat "$logFile" | grep "^01"  | sed 's/01\t.*\t//g' | sed 's/ <[a-zA-Z0-9]*>$//g' > /tmp/strtest
		if [[ -s /tmp/strtest ]] ;then
			echo >> /critical.log
			echo "---Critical errors for $logFile" >> /critical.log
			echo "---Critical errors for $logFile" >> /tmp/status.log
			echo >> /critical.log
			cat /tmp/strtest | sort -u | while read line ;do echo "$line [`cat /tmp/strtest | grep -F "$line" | wc -l`]" >> /critical.log ;done
		fi
		echo "---Critical errors for $logFile done" >> /tmp/status.log
		#rm -f /tmp/strtest
	fi
done
