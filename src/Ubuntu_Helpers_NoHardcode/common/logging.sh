#!/bin/bash

. /etc/lmce-build/builder.conf

exec 100>&1
exec >"$log_file"
exec 2>&1

function DisplayMessage {
	echo "`date +%H:%M:%S`  $*" >&100
}

function Error {
	set +x
	echo
	DisplayMessage "ERROR: $*"
	echo			>&100
	echo 			>&100
	tail -20 "$log_file"	>&100
	echo 			>&100

	exit 1
}
