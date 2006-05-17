#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

LogFile="$1"

if [[ -z "$LogFile" ]]; then
	echo "You must specify the log file where you want the section extracted from"
	exit 1
fi

# Line number where last delimiter is seen
Line=$(cat "$LogFile" | grep -nF "$LogSectionDelimiter" | tail -1 | cut -d: -f1)

# Display everything from the last delimiter to the end of file
# If delimiter is missing, display the entire file
awk -vline="$Line" 'NR>line' "$LogFile"
