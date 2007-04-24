#!/bin/bash -e
. /tmp/mce_wizard_data.sh

FROM_CD=1
FROM_ISO=2

CD_Dir=$(mktemp -d)

if [[ "$c_linuxmceCdFrom" == "$FROM_ISO" ]] ;then
	mount -o loop "$c_linuxmceCdIsoPath" "$CD_Dir" || exit 1
else
	mount /dev/cdrom "$CD_Dir" || exit 4
fi

mkdir -p /usr/pluto/deb-cache || exit 2
cp "${CD_Dir}"/*.deb /usr/pluto/deb-cache || exit 3
cp "${CD_Dir}"/Packages* /usr/pluto/deb-cache/ || exit 5

umount -lf $CD_Dir || :
rm -rf $CD_Dir

exit 0
