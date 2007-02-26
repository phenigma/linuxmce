#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

RetrySend()
{
	local Mac="$1" IP="$2"
	while ! /usr/pluto/bin/MessageSend "$DCERouter" "$PK_Device" -1001 2 65 52 3 53 2 5 "$MAC" 28 "$IP"; do
		sleep 1
	done
}

while read mac ip garbage; do
	RetrySend "$mac" "$ip" &
done
