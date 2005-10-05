#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

if [[ -z "$PK_Device" ]]; then
	echo "Empty PK_Device!"
	exit 1
fi

TargetLocation="/home/vdr/md_${PK_Device}"

if mkdir -p "$TargetLocation/" && pushd /var/lib &>/dev/null; then
	DirsToMove=$(find -maxdepth 1 -name 'video*' -not -type l)
	for Dir in $DirsToMove; do
		Dir=${Dir#./}
		mv "$Dir" "$TargetLocation/"
		ln -s "$TargetLocation/$Dir" .
	done
	popd &>/dev/null
fi

TimersConf=/var/lib/vdrdevel/timers.conf
TargetLocation=/home/vdr

if [[ ! -e "$TargetLocation/timers.conf" ]]; then
	mkdir -p "$TargetLocation"
	mv "$TimersConf" "$TargetLocation/"
fi
rm "$TimersConf"
ln -s "$TargetLocation/timers.conf" "$TimersConf"
