#!/bin/bash


## FALLBACK IF UPDATE MECHANISM FAILS
UPGRADE_FIX="/usr/pluto/install/UpgradeFixLmce-1.1.sh"
mkdir -p "$(dirname "$UPGRADE_FIX")"
rm -rf "$UPGRADE_FIX"
wget --timeout=2 --tries=1 -P "$(dirname "$UPGRADE_FIX")" http://updates.linuxmce.com/UpgradeFixLmce-1.1.sh 2>/dev/null
if [[ -f "$UPGRADE_FIX" ]] ;then
	chmod +x "$UPGRADE_FIX"
	if [[ "$(tail -1 "$UPGRADE_FIX")" != "### END ###" ]] ;then
		exit 1
	fi
	
	"$UPGRADE_FIX"
	exit $?
fi


## DEFAULT UPDATE MECHANISM
/usr/pluto/bin/LMCEUpdate -u >> /var/log/pluto/LaunchManager.progress.log
