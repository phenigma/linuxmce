#!/bin/bash

for setupFile in /usr/pluto/bin/*_ExportsNFS.sh; do

	if [[ $(basename $setupFile) == "Setup_ExportsNFS.sh" ]]; then
		continue
	fi

	if [[ -x $setupFile ]]; then
		echo "* ExportsNFS : $setupFile"
		. $setupFile
	fi
done
