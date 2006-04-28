#!/bin/bash

timeZone="$1"

## Function for seting timezone for a computer
function setTimezone() {
	rootDir=$1
	timeZone=$2
	
	zoneFile="/usr/share/zoneinfo/$TimeZone"
	[[ -z "$timeZone" || ! -f "${rootDir}${zoneFile}" ]] && exit 1

	rm -f $rootDir/etc/localtime
	ln -s "$zoneFile" $rootDir/etc/localtime
	
	echo "$timeZone" >$rootDir/etc/timezone
}

## Set timezone for core
setTimezone "/" $timeZone

## Set timezone for MDs
for rootDir in /usr/pluto/diskless/* ;do
	setTimezone $rootDir $timeZone
done
