#!/bin/bash

Source="$1"
Link="$2"

if [[ ! -e "$Source" ]]; then
	echo "1st parameter does not exist"
	exit 1
fi

if [[ -e "$Link" && ! -L "$Link" ]]; then
	echo "2nd parameter not a link"
	exit 1
fi

rm -f "$Link"
if ! ln -s "$Source" "$Link"; then
	echo "Failed to make link"
	exit 1
fi

exit 0
