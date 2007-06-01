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

function unpack_filesystem {
	if [[ ! -e ${Moon_RootLocation}/var/lib/dpkg/status ]] ;then
		mkdir -p "${Moon_RootLocation}"	
		pushd $Moon_RootLocation
			tar -xf $Moon_RootArchive
		popd
	fi
}

function is_image_upgradable {
	## Test to see if the image is upgradable
	cp /etc/resolv.conf ${Moon_RootLocation}/etc/resolv.conf
	cp /etc/apt/sources.list ${Moon_RootLocation}/etc/apt/sources.list

	chroot $Moon_RootLocation apt-get update 2>/dev/null 1>/dev/null
	UpgradablePkgs=$(chroot $Moon_RootLocation apt-get -s dist-upgrade 2>/dev/null | grep "^Conf\|^Inst" | wc -l)
	
	if [[ "$UpgradablePkgs" != "0" ]] ;then
		return 0
	else
		return 1
	fi
}

if [[ ! -f $Moon_RootArchive ]] ;then
	/usr/pluto/bin/Diskless_CreateTBZ.sh
	unpack_filesystem

else
	unpack_filesystem
	
fi
