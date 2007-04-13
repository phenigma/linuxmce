#!/bin/bash

LogInterval=5 # minutes

Count=0
while :; do
	if [[ "$Count" -eq 0 ]]; then
		Count=$LogInterval
		DateComponent=$(date +%Y%m%d-%H%M%S)
		./top-batch.sh >/var/log/pluto/top-watch."$DateComponent".log
	fi
	sleep 60
	((Count--))
done
