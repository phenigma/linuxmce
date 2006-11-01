#!/bin/bash

lastline=
while read line; do
	echo "$line"
	lastline="$line"
done
echo "$lastline" >/tmp/rip_message
