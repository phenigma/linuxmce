#!/bin/bash

MythPkg=(
	libmyth-0.20 mythtv-backend mythtv-common mythtv-database mythtv-frontend mythtv-transcode-utils
	mythweb pluto-mythtv-player pluto-mythtv-plugin
)

VDRPkg=(
	lmcevdr libxine-xvdr vdr vdr-dev vdr-plugin-control vdr-plugin-xineliboutput
	libxineliboutput-sxfe xineliboutput-sxfe pluto-vdr pluto-vdr-plugin
)

RemovePkg()
{
	local Pkg

	for Pkg in "$@"; do
		apt-get -y -f remove "$Pkg"
	done
}

Selection="$1"

. /usr/pluto/bin/LockUtils.sh

trap 'Unlock "CDC" "remove_pvr_packages"' EXIT
WaitLock "CDC" "remove_pvr_packages" # don't run in paralel with CDC

case "$Selection" in
	mythtv)
		RemovePkg "${MythPkg[@]}"
	;;
	vdr)
		RemovePkg "${VDRPkg[@]}"
	;;
	*)
		echo "Unknown selection: '$Selection'"
	;;
esac
