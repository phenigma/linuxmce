#!/bin/bash

. /usr/pluto/bin/pluto.func

set -e

if [[ "$#" -ne 1 ]]; then
	echo "Syntax: $0 <Device>"
	exit 1
fi

Moon_DeviceID="$1"
if [[ "$Moon_DeviceID" == "" ]] ;then
	echo "ERROR: Script $0 called with missing parameter"
	exit 1
fi

Moon_RootLocation="/usr/pluto/diskless/${Moon_DeviceID}"
Moon_RootArchive="/usr/pluto/install/PlutoMD.tar.bz2"

## Create Root Directory
mkdir -p "${Moon_RootLocation}"

## Unpack Diskless Filesystem
pushd $Moon_RootLocation
	tar -xvf $Moon_RootArchive
popd


