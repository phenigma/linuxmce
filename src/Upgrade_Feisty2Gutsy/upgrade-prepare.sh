#!/bin/bash

TMP_FILE="$(mktemp)"

FROM_DVD=1
FROM_NET=2

function Error {
	local Message="$*"
	whiptail \
		--title 'LinuxMCE Release Upgrade' \
		--msgbox "ERROR :\n$Message" \
		10 40

	exit 1
}

## First menu
whiptail \
	--title  'LinuxMCE Release Upgrade' \
	--menu "Chose Upgrade Method" \
	10 50 2 \
	"$FROM_DVD" 'Use LinuxMCE 0710 DVD to get the packages' \
	"$FROM_NET" 'Download the packages from the Internet' \
	2> "$TMP_FILE"
InstallFrom=$(cat "$TMP_FILE")


## Extract deb-cache from dvd
if [[ "$InstallFrom" == "$FROM_DVD" ]] ;then
	whiptail --title 'LinuxMCE Release Upgrade' --clear --msgbox "Please insert the LinuxMCE 0710 DVD" 10 40
	clear

	echo "Mounting LinuxMCE DVD"
	sleep 0.5
	TEMP_DIR="$(mktemp -d)"
#	mount /dev/cdrom "$TEMP_DIR" || Error "Cannot mount LinuxMCE 0710 DVD"
	sleep 1

	echo "Copying Packages from the DVD"
	mkdir -p /usr/pluto/deb-cache-new/
#	cat "$TEMP_DIR/linux-mce.tar.gz_0"* | tar zxv --checkpoint --strip 4 -C /usr/pluto/deb-cache-new './usr/pluto/deb-cache' || Error "There was a problem while reading the DVD"

	whiptail --title "LinuxMCE Release Upgrade" --clear --msgbox "Copied OK.\n\nYou need to reboot your computer to start the upgrade proccess." 10 50
	exit 0	
else
	whiptail --title 'LinuxMCE Release Upgrade' --clear --msgbox "The dowload will begin in background. You will be prompted in Orbiter when the download will finish." 10 50
	clear
fi

rm -rf "$TMP_FILE"
