#!/bin/bash
skip=9
echo > /tmp/mce-wget-progress
while read line; do
	if [[ "$skip" != "0" ]] ;then
		skip=$(($skip - 1))
	else
		echo $line | sed -u 's/  */ /g' | sed -u 's/^ //g' | sed -u 's/%//g' | awk '{print $7 "|" $1 "|" $8 " " $9}'  >> /tmp/mce-wget-progress
	fi
done < <(wget -c -P /tmp --progress=dot  http://deb.plutohome.com/debian/dists/20dev_via/installation-cd-1-2.0.0.43_via.iso 2>&1 1>/dev/null)
