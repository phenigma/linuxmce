#!/bin/bash

count=1
top="$(top -b -n 1)"
while read; do
	if [[ "$count" -le 7 ]]; then
		header="$header$REPLY\n"
	else
		output="$output$REPLY\n"
	fi
	((count++))
done < <(echo "$top")
echo -en "$header"
echo -en "$output" | sort -nrk10
