#!/bin/bash

. /etc/lmce-build/builder.conf

exec 100>&1
exec >"$log_file"
exec 2>&1

function DisplayMessage {
	echo "`date +%H:%M:%S`  $*" >&100
}

function Error {
	echo
	DisplayMessage "ERROR: $*"
	tail -20 "$log_file"

	exit 1
}
