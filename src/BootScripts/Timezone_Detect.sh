#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

if [[ "$TimeZoneSet" == 1 ]]; then
	exit # timezone has already been set once
fi

TimeZone=$(wget --timeout=10 -O - http://timezone.plutohome.com/geoip/detect_timezone.php)
if [[ -z "$TimeZone" ]]; then
	exit # timezone could not be detected
fi

/usr/pluto/bin/SetTimeZone.sh "$TimeZone"
ConfSet TimeZoneSet 1
