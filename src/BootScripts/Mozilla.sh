#!/bin/bash

Moz=/usr/bin/mozilla
Rat=/usr/pluto/bin/ratpoison
export DISPLAY=:0
Orbiter="$1"
URL="$2"

MozId="$($Rat -c 'windows %n %c' | grep -F Firefox-bin | head -1 | cut -d' ' -f1)"
if [ -z "$MozId" ]; then
	$Moz "$URL"
	/usr/pluto/bin/MessageSend 0 "$Orbiter" 1 4 16 1927
else
	$Rat -c "select $MozId"
	$Moz -remote "openurl($URL)"
fi
