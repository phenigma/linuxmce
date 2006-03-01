#!/bin/bash

## Get a list with all the devices 
deviceList=$(df | grep "^/dev/" | cut -d' ' -f1)

## For eache device
for device in $deviceList; do
	deviceSize=$(df $device | awk '/^\/dev\//{ print $2 }')
	deviceUsed=$(df $device | awk '/^\/dev\//{ print $3 }')
	deviceAvail=$(df $device | awk '/^\/dev\//{ print $4 }')
	deviceProcUsed=$(df $device | awk '/^\/dev\//{ gsub(/%$/,"",$5); print $5 }')
	deviceMounted=$(df $device | awk '/^\/dev\//{ print $6 }')

	## Free space
	if [[ $deviceProcUsed -gt 95 ]]; then
		#TODO: Send information about low disk space 
		#      or even do a cleanup here
	fi
done
