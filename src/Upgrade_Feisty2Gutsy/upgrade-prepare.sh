#!/bin/bash

FROM_DVD=1
FROM_NET=2

COLOUR_WHITE="\033[1;37m"
COLOUR_RED="\033[1;31m"
COLOUR_NONE="\033[0m"

function Error {
	local Message="$*"

	echo	
	echo -e "${COLOUR_RED}ERROR : $Message ${COLOUR_NONE}"
	echo
	read
	exit 1
}

function Message {
	local Message="$*"
	echo -e "${COLOUR_WHITE}${Message}${COLOUR_NONE}"
}

clear

InstallFrom=0
while [[ "$InstallFrom" != "$FROM_DVD"  && "$InstallFrom" != "$FROM_NET" ]] ;do
	echo "Chose Upgrade Method:"
	echo " 1 - Use LinuxMCE 0710 DVD to get the packages"
	echo " 2 - Download the packages from the Internet"
	echo
	echo -n "Option: "
	read InstallFrom
	echo
done


if [[ "$InstallFrom" == "$FROM_DVD" ]] ;then
	echo "Please insert the LinuxMCE 0710 DVD and press ENTER ..."
	read
	clear

	Message "* Mounting LinuxMCE DVD"
	sleep 0.5
	TEMP_DIR="$(mktemp -d)"
	mount /dev/cdrom "$TEMP_DIR" || Error "Cannot mount LinuxMCE 0710 DVD"
	sleep 1

	Message "* Copying package from DVD"
	mkdir -p /usr/pluto/deb-cache-new/ || Error "Cannot create target directory on disk"

	test -f "$TEMP_DIR/linux-mce.tar.gz_01"  || Error "Cannout find LinuxMCE 0710 image on the DVD"
	test -f "$TEMP_DIR/linux-mce.tar.gz_02"  || Error "Cannout find LinuxMCE 0710 image on the DVD"

	cat "$TEMP_DIR/linux-mce.tar.gz_0"* | tar zxv --checkpoint --strip 4 -C /usr/pluto/deb-cache-new './usr/pluto/deb-cache' || Error "There was a problem while reading the DVD"

	Message "*  You need to reboot your computer to start the upgrade process"
	read
	exit 0
fi


if [[ "$InstallFrom" == "$FROM_NET" ]] ;then
	Message "The dowload will begin in background. You will be prompted in Orbiter when the download will finish."
	read
	exit 0
fi

