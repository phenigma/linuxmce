#!/bin/bash

FROM_DVD=1
FROM_NET=2

COLOUR_WHITE="\033[1;37m"
COLOUR_RED="\033[1;31m"
COLOUR_NONE="\033[0m"

export PROMPT_COMMAND="echo -ne 'LinuxMCE 0704 to 0710 Upgrade'"
clear

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


# Test to see if the user didn't have started the download already
Option=""
if [[ -f /etc/cron.d/gutsy-upgrade-scripts ]] ;then
	while : ;do
		echo "You already selected to download the packages from the internet. What you want to do:"
		echo " 1 - Show the download status"
		echo " 2 - Stop the download, i want to chose again"
		echo " 3 - Exit"
		echo
		echo -n "Option: "
		read Option
		echo

		# Show download status
		if [[ "$Option" == "1" ]] ;then
			clear
			tail  /var/gutsy-upgrade-scripts/download.log 2>/dev/null || echo 'Download not started yet'
			echo
			echo "Press ENTER to return to menu"
			read
			clear
		fi
		
		# Stop the download 
		if [[ "$Option" == "2" ]] ;then
			rm -f /etc/cron.d/gutsy-upgrade-scripts &>/dev/null
			kill `cat /var/gutsy-upgrade-scripts/download.pid` &>/dev/null
			rm /var/gutsy-upgrade-scripts/download.log
			clear
			break
		fi

		# Exit
		if [[ "$Option" == "3" ]] ;then
			exit 0
		fi

	done
fi



# Ask the user from where he wants to download deb-cache
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


# If the user chose to use the dvd to install deb-cache
if [[ "$InstallFrom" == "$FROM_DVD" ]] ;then
	# Ask the user to insert the dvd
	echo "Please insert the LinuxMCE 0710 DVD and press ENTER ..."
	read
	clear

	# Mount the dvd
	Message "* Mounting LinuxMCE DVD"
	sleep 0.5
	TEMP_DIR="$(mktemp -d)"
	mount /dev/cdrom "$TEMP_DIR" || Error "Cannot mount LinuxMCE 0710 DVD"
	sleep 1

	# Prepare to copy deb-cache
	Message "* Copying package from DVD"
	mkdir -p /usr/pluto/deb-cache-new/ || Error "Cannot create target directory on disk"
	test -f "$TEMP_DIR/lmce-image/linux-mce.tar.gz_01"  || Error "Cannout find LinuxMCE 0710 image on the DVD"

	# Copy deb-cache
	cat "$TEMP_DIR/lmce-image/linux-mce.tar.gz_0"* | tar zxv --checkpoint --strip 4 -C /usr/pluto/deb-cache-new './usr/pluto/deb-cache' || Error "There was a problem while reading the DVD"

	# Copy diskless images
	cp "$TEMP_DIR/diskless-images"/PlutoMD-*.tar.bz /usr/pluto/install

	# Copy crossover deb-cache
	cp "$TEMP_DIR/deb-cache"/*.deb /usr/pluto/deb-cache-new/

	# Copy diskless images
	cp "$TEMP_DIR/diskless-images/PlutoMD*.tar.bz2" /usr/pluto/install || Error "Cannot copy Diskless Images from the DVD"

	# Get out of here
	touch /var/gutsy-upgrade-scripts/upgrade-ready
	Message "*  You need to reboot your computer to start the upgrade process"
	read
	exit 0
fi

# If the user chosed to download deb-cache from internet
if [[ "$InstallFrom" == "$FROM_NET" ]] ;then

	# Add a entry in cron to start/restart the download every half an hour
	echo '*/30 *  * * * /usr/share/gutsy-upgrade-scripts/scripts/upgrade-download.sh' > /etc/cron.d/gutsy-upgrade-scripts

	# Start the download now also
	/usr/share/gutsy-upgrade-scripts/scripts/upgrade-download.sh

	# Get out of here
	Message "The dowload will begin in background. You will be prompted in Orbiter when the download will finish."
	read
	exit 0
fi

