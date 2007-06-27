#!/bin/bash -e

. /tmp/mce_wizard_data.sh

FROM_CD=1
FROM_ISO=2

if [[ -z $1 ]] && [[ "$c_ubuntuExtraCdFrom" == "$FROM_ISO" ]] ;then
        [ -e "$c_ubuntuExtraCdPath" ] || exit 5
else

	CD_Dir=$(mktemp -d)

	if [[ "$c_ubuntuExtraCdFrom" == "$FROM_ISO" ]] ;then
		mount -o loop "$c_ubuntuExtraCdPath" "$CD_Dir" || exit 1
	else
		mount /dev/cdrom "$CD_Dir" || exit 4
	fi
	trap "umount -lf \"$CD_Dir\"" EXIT

	mkdir -p /usr/pluto/deb-cache || exit 2
	cp "${CD_Dir}"/cachecd1-cache/*.deb /usr/pluto/deb-cache || exit 3

	cp ./dpkg-scanpackages /usr/pluto/deb-cache || :
	pushd /usr/pluto/deb-cache
		./dpkg-scanpackages ./ /dev/null > Packages
		gzip -c Packages > Packages.gz
	popd

	dpkg -i /usr/pluto/deb-cache/video-wizard-videos_*.deb || :

	umount -lf $CD_Dir || :
	rm -rf $CD_Dir

	trap "" EXIT

	exit 0
fi
