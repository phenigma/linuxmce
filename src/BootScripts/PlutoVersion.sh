#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

. /usr/pluto/bin/PlutoVersion.h

Q="SELECT FK_DeviceCategory FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device=$PK_Device"
DeviceCategory=$(RunSQL "$Q")

DeviceCategory_Core=7
DeviceCategory_MD=8

trap : SIGINT

if [[ -z "$DeviceCategory" ]]; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Can't determine Device Category"
elif [[ $DeviceCategory -eq $DeviceCategory_MD ]]; then
	CoreVersion="$(</home/pluto-version)"
	if [[ "$Version" != "$CoreVersion" ]]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "*************************************************"
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Core and MD version are different."
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Core version:           $CoreVersion"
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "This machine's version: $Version"
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "It's very possible that this MD updated and is now a newer version than the Core is. To match versions again, you need to reboot the Core so it updates too."
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "*************************************************"
		Logging "$TYPE" "$SEVERITY_WARNING" "$0" "The Media Director won't boot until it matches the Core's version"
		while :; do
			sleep 1
		done
	fi
fi
