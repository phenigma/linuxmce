#!/bin/bash

# PLEASE, DO NOT DELETE THIS SCRIPT

# Note:
#   This script translates a UniqID, as provided by the ListVideoDevices.sh
#   script, to a /dev/video* entry, usable by motion and other video capture
#   software.
#
# These two scripts were written to ease the manipulation of video capture
# hardware in the case where there are two or more such devices and chances
# are that they switch positions between reboots, unplug/replug cycles, etc.
#
#                                    -- Radu Cristescu

Err()
{
	echo "$@" >&2
	exit 1
}

for ((i = 1; i <= $#; i++)); do
	case "${!i}" in
		--uniqid) ((i++)); UniqID="${!i}" ;;
		*) Err "Unknown parameter '${!i}'"
	esac
done

if [[ -z "$UniqID" ]]; then
	Err "You must provide an ID to translate via the --uniqid parameter"
fi

./ListVideoDevices.sh | grep -F "$UniqID" | cut -d' ' -f2
