#!/bin/bash

for setupFile in /usr/pluto/bin/StorageDevices_*; do

	if [[ $(basename $setupFile) == "StorageDevices_Setup.sh" ]]; then
		continue
	fi

	if [[ -x $setupFile ]]; then
		echo "* StorageDevices : $setupFile"
		. $setupFile
	fi
done
