#!/bin/bash

PID=$$
cmd_line="$1"
echo "$PID $cmd_line (by $0)" >>/var/log/pluto/running.pids
exec $*
