#!/bin/bash

for UpgradePatch in /usr/share/pluto-upgrade-helper/* ;do
	[[ -x $UpgradePatch ]] || continue

	echo "Running $UpgradePatch"
	. $UpgradePatch || /bin/true
done
