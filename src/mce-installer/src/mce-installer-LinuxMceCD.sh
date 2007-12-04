#!/bin/bash -e
. /tmp/mce_wizard_data.sh

FROM_CD=1
FROM_ISO=2

if [[ -z $1 ]] && [[ "$c_linuxmceCdFrom" == "$FROM_ISO" ]] ;then
	[ -e "$c_linuxmceCdIsoPath" ] || exit 5
else 
	CD_Dir=$(mktemp -d)

	if [[ "$c_linuxmceCdFrom" == "$FROM_ISO" ]] ;then
		mount -o loop "$c_linuxmceCdIsoPath" "$CD_Dir" || exit 1
	else
		mount /dev/cdrom "$CD_Dir" || exit 4
	fi
	trap "umount -f \"$CD_Dir\"" EXIT

	mkdir -p /usr/pluto/deb-cache || exit 2
	cp "${CD_Dir}"/deb-cache/*.deb /usr/pluto/deb-cache || exit 3

	cp ./dpkg-scanpackages /usr/pluto/deb-cache || :
	cp ./controllib.pl /usr/pluto/deb-cache || :
	pushd /usr/pluto/deb-cache
		./dpkg-scanpackages ./ /dev/null > Packages
		gzip -c Packages > Packages.gz
	popd

	umount -f $CD_Dir || :
	rm -rf $CD_Dir

	trap "" EXIT

	exit 0
fi
