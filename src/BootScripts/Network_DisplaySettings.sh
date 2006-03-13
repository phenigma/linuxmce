#!/bin/bash

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--orbiter) Orbiter=y ;;
		*) echo "Unknown option '${!i}'" ;;
	esac
done

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh

if [[ "$Orbiter" == y ]]; then
	case "$DCERouter" in
		localhost) RouterIP="127.0.0.1" ;;
		dcerouter) RouterIP="192.168.80.1" ;; # TODO: remove hardcoded assumption
		*) RouterIP="$DCERouter" ;;
	esac
	echo "router=$RouterIP; internal=$IntIP($IntIf); external=$ExtIP($ExtIf)"
else
	echo "DCE Router: $DCERouter"
	echo "Internal interface: $IntIf; IP: $IntIP"
	echo "External interface: $ExtIf; IP: $ExtIP"
fi
