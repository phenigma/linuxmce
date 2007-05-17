#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

#if [[ "$TimeZoneSet" == 1 ]]; then
#	exit # timezone has already been set once
#fi

Info=$(wget --timeout=10 -O - http://timezone.plutohome.com/geoip/detect_timezone.php)
if [[ -z "$Info" ]]; then
	exit # timezone/location could not be detected
fi

TimeZone=$(echo "$Info" | cut -f1)
PK_City=$(echo "$Info" | cut -f2)
City=$(echo "$Info" | cut -f3)
Region=$(echo "$Info" | cut -f4)
PK_Country=$(echo "$Info" | cut -f5)

if [[ -z "$PK_City" || -z "$PK_City" || -z "$City" || -z "$Region" || -z "$PK_Country" ]]; then
	exit # timezone/location could not be detected
fi


Query="UPDATE Installation SET FK_City=$PK_City, City=\"$City\", State=\"$Region\", FK_Country=$PK_Country"

RunSQL "$Query"

/usr/pluto/bin/SetTimeZone.sh "$TimeZone"
ConfSet TimeZoneSet 1
invoke-rc.d ntpdate restart
