#!/bin/bash

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--orbiter) Orbiter=y ;;
		*) echo "Unknown option '${!i}'" ;;
	esac
done

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

case "$DCERouter" in
	localhost) RouterIP="127.0.0.1" ;;
	dcerouter) 
		Q="SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7"
		RouterIP=$(RunSQL "$Q")
		if [[ "$RouterIP" == "" ]] ;then
			 RouterIP="192.168.80.1"
		fi
	;; # TODO: remove hardcoded assumption
	*) RouterIP="$DCERouter" ;;
esac

if [[ "$ExtIP" == dhcp ]]; then
	ExtIP="$ExtIPreal dhcp"
fi

if [[ "$Orbiter" == y ]]; then
	if [[ -n "$IntIf" ]]; then
		echo "router=$RouterIP; internal=$IntIP($IntIf); external=$ExtIP($ExtIf)"
	else
		echo "router=$RouterIP; this MD=$ExtIP"
	fi
else
	echo "DCE Router: $DCERouter"

	if [[ -n "$IntIf" ]]; then
		echo "Internal interface: $IntIf; IP: $IntIP"
		echo "External interface: $ExtIf; IP: $ExtIP"
	else
		echo "this MD: $ExtIP"
	fi
fi
