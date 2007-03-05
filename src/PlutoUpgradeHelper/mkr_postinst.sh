#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

PrevVer="$2"
for UpgradePatch in /usr/share/pluto-upgrade-helper/* ;do
	[[ -x "$UpgradePatch" ]] || continue

	echo "Running '$UpgradePatch'"
	"$UpgradePatch" "$PrevVer" || /bin/true
done
