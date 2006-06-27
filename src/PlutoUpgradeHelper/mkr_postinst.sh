#!/bin/bash

for UpgradePatch in /usr/share/pluto-upgrade-helper/* ;do
	[[ -x $UpgradePatch ]] || continue

	. $UpgradePatch || /bin/true
done
