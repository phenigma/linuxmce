#!/bin/bash

. /usr/share/debconf/confmodule

while read owner variable type value; do
	if [[ -z "$owner" || "$owner" == "#"* || "$owner" == "d-i" || "$owner" == "debconf" ]]; then
		continue
	fi
	preseed=("${preseed[@]}" "$variable=$value")
	# can't call db_set here because we "highjacked" the standard input and debconf doesn't like that
done </usr/pluto/install/preseed.cfg

# do the debconf things here
for setting in "${preseed[@]}"; do
	variable="${setting%%=*}"
	value="${setting#*=}"
	db_set "$variable" "$value"
	db_fset "$variable" seen true
done
