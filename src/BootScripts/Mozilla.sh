#!/bin/bash

Moz=/usr/bin/mozilla
Rat=/usr/pluto/bin/ratpoison
export DISPLAY=:0
URL="$1"

MozId="$($Rat -c 'windows %n %c' | grep -F Firefox-bin | head -1 | cut -d' ' -f1)"
if [ -z "$MozId" ]; then
	$Moz "$URL"
else
	$Rat -c "select $MozId"
	$Moz -remote "openurl($URL)"
fi
