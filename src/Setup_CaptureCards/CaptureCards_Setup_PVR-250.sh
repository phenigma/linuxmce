#!/bin/bash

. /usr/pluto/bin/CaptureCards_Utils.sh

Device="$1"
Slot="$2"

UpdatePorts()
{
	local Device="$1" Slot="$2"

	Skip=1

	while read line; do
		if [[ "$line" == card:* ]]; then
			FoundSlot="${line##*bus info }"
			FoundSlot="${FoundSlot%%, ivtv*}"
			FoundSlot="${FoundSlot#*:}"

			if [[ "$FoundSlot" == "$Slot" ]]; then
				Skip=0
			fi
		elif [[ "$Skip" -eq 0 ]]; then
			if [[ "$line" != *"MPG encoding"* ]]; then
				DevEntry="#ERROR: device doesn't support MPEG encoding"
			else
				DevEntry="${line##*/dev/}"
				DevEntry="${DevEntry%%: MPG*}"
			fi
			Skip=1
		fi
	done < <(ivtv-detect | egrep '^card:|/dev/.*: MPG encoding')

	UpdatePorts_NoFind "$Device" "$DevEntry"
}

UpdatePorts "$Device" "$Slot"
