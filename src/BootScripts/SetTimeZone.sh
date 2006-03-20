#!/bin/bash

. /usr/pluto/bin/pluto.func

TimeZone="$1"

Logging "$TYPE" "$SEVERITY_NORMAL" "Timezone" "Setting timezone to $TimeZone"

File="/usr/share/zoneinfo/$TimeZone"
[[ -z "$TimeZone" || ! -f "$File" ]] && exit 1

rm -f /etc/localtime
ln -s "$File" /etc/localtime
echo "$TimeZone" >/etc/timezone
