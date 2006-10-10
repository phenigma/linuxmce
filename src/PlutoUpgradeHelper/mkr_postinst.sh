#!/bin/bash

PrevVer="$2"
for UpgradePatch in /usr/share/pluto-upgrade-helper/* ;do
	[[ -x "$UpgradePatch" ]] || continue

	echo "Running '$UpgradePatch'"
	"$UpgradePatch" "$PrevVer" || /bin/true
done
