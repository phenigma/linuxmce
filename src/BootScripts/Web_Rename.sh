#!/bin/bash

#
# INFO: Wrapper to mv used by pluto admin to rename files
#
# USAGE: Web_Rename,sh <SOURCE_FILE> <DESTINATION_FILE>
#

srcFile=$1
dstFile=$2

## A array of regexes containing the safe dirs
safeDirs=( "/home/public/data/" )

## Check to see if the files are stored in safe directories
srcSafe=false
dstSafe=false
for i in `seq 0 $((${#safeDirs[@]} - 1))` ;do
	safeDir=${safeDirs[$i]}

	echo "$srcFile" | grep -q "^$safeDir"
	if [[ "$?" == "0" ]] ;then
		srcSafe=true
	fi

	echo "$dstFile" | grep -q "^$safeDir"
	if [[ "$?" == "0" ]] ;then
		dstSafe=true
	fi
done

## Issue a warning if unsafe, useful for debuging
if [[ "$srcSafe" == false ]] ;then
	echo "Warning: Source is located in an unsafe directory"
fi

if [[ "$dstSafe" == false ]] ;then
	echo "Warning: Destination is located in an unsafe directory"
fi

## If both dirs are safe, run mv with actual priviledges
if [[ "$dstSafe" == "true" && $srcSafe == "true" ]] ;then
	mv "$srcFile" "$dstFile"
	exit $?

## If at lease one of the dirs is not safe do the sudo test
else
	## If runned using sudo, use the rights of the one who invoked the command
	if [[ "$SUDO_USER" != "" ]] ;then
		sudo -u $SUDO_USER mv "$srcFile" "$dstFile"
		exit $?

	# If not runned with sudo, just pass the parameters to mv
	else
		mv "$srcFile" "$dstFile"
	fi
fi
