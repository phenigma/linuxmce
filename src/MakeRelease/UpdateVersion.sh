#!/bin/bash

if [ "$1" = "" ]; then
	Q="select PK_Version from Version WHERE PK_Version<>1 ORDER BY date desc, PK_Version limit 1"
	last_version=$(echo "$Q;" | mysql -N pluto_main)

	Q="select VersionName from Version WHERE PK_Version=$last_version"
	last_version_name=$(echo "$Q;" | mysql -N pluto_main)

	O1="UPDATE Version SET VersionName='${last_version_name}.$(date +%g%m%d%H)' WHERE PK_Version=$version;"
	echo "$O1" | mysql pluto_main
fi

