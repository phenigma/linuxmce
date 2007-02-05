#!/bin/bash

. /usr/pluto/bin/pluto.func

set -e

if [[ "$#" -ne 4 ]]; then
	echo "Syntax: $0 <Device>"
	exit 1
fi
Moon_DeviceID="$3"
Moon_RootLocation="/usr/pluto/diskless/${Diskless_DeviceID}"
Moon_RootArchive="/usr/pluto/install/PlutoMD.tar.bz2"

## Create Root Directory
mkdir -p "${Moon_RootLocation}"

## Unpack Diskless Filesystem
pushd $Moon_RootLocation
	tar -xvf $Moon_RootLocation
popd


