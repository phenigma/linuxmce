#!/bin/bash

. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Killing these processes: $*"
for Pid in "$@"; do
	Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Killing PID $Pid"
	kill -9 $Pid 2>&1
done
