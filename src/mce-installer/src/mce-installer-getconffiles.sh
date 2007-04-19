#!/bin/bash

lines=$(find /etc/ -name '*.pbackup')

for line in $lines; do
	echo "$line" | sed 's/.pbackup//g'
done

