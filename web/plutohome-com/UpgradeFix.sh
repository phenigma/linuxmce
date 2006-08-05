#!/bin/bash

DceRouterInstalled=$(aptitude show pluto-dcerouter | grep "^State:" | cut -d':' -f2)

if [[ "$DceRouterInstalled" != " not installed" ]] ;then
	
	if [[ -f /usr/pluto/var/Updates/DownloadUpdates.lock ]]; then
		# This machine is the Core => Powered off while DownloadUpdates.sh
		# was runnig. We should clean up.
		rm -f /usr/pluto/var/Updates/DownloadUpdates.lock
	fi

	## Test if is ok to apply updates now else skip
	if [[ ! -f /usr/pluto/var/Updates/UpdatesOk.flag ]]; then
		exit 0
	else
		UpdatesOkStamp=$( cat /usr/pluto/var/Updates/UpdatesOk.flag )
	fi
	
	## What is our version now, should we update
	LastUpdateStamp=0
	if [[ -f /etc/lastUpdate.stamp ]]; then
	        LastUpdateStamp=$( cat /etc/lastUpdate.stamp )
	fi

	if [[ "$LastUpdateStamp" == "$UpdatesOkStamp" ]]; then
		exit 0
	fi
																													
	splash -s -u 0
	clear
	echo " ---------------------------------------------------------------------------"
	echo
	echo "  Starting Core system upgrade in 10 seconds. "
	echo
	echo "  IMPORTANT: Don't reboot your computer until the update is completed ! "
	echo 
	echo " ---------------------------------------------------------------------------"
	sleep 10

	/usr/sbin/alsactl restore || /bin/true
	
	## Update to be sure we get the latest files
	apt-get update

	## This needs to be installed first else some packages get removed
	apt-get -y upgrade

	
	PlutoXinePlayer=$(aptitude show pluto-xine-player | grep "^State:" | cut -d":" -f2)
	if [[ "$PlutoXinePlayer" != " not installed" ]] ;then	
		apt-get -y install libxine1
	fi
	
	## Now try to do a normal dist upgrade
	apt-get -y -f dist-upgrade

	Q="UPDATE Device SET NeedConfigure = '1' WHERE FK_DeviceTemplate='28'"
	echo "$Q" | mysql pluto_main -A -N

	echo "- Copying kernel package(s) for later use"
	cp /var/cache/apt/archives/linux-image* /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ 2>/dev/null

	clear
	echo " ---------------------------------------------------------------------------"
	echo 
	echo "  Upgrade Completed, system is going to reboot after 10 seconds !!          "
	echo
	echo " ---------------------------------------------------------------------------"
	sleep 10
	
	init 6
	
else

	## Check to see it the core updated and updated out kernel too
	Kernel40=$(aptitude show linux-image-2.6.16.20-pluto-1-686 | grep "^State:" | cut -d ":" -f2)
	if [[ $Kernel40 == " installed" ]] ;then
		splash -s -u 0
		clear
		echo " ---------------------------------------------------------------------------"
		echo
		echo "  Starting Media Director system upgrade in 10 seconds. "
		echo
		echo "  IMPORTANT: Don't reboot your computer until the update is completed ! "
		echo 
		echo " ---------------------------------------------------------------------------"
		sleep 10
	
		/usr/sbin/alsactl restore || /bin/true
	

		apt-get update
		dpkg --purge --force-all hotplug	
		apt-get -f -y install hal udev dbus dbus-1 libhal1 libdbus-1-2 libselinux1 libsepol1 libdbus-glib-1-2
		apt-get -y install mythtv-backend
		apt-get -y -f dist-upgrade


		clear
		echo " ---------------------------------------------------------------------------"
		echo 
		echo "  Upgrade Completed, system is going to reboot after 10 seconds !!          "
		echo
		echo " ---------------------------------------------------------------------------"
		sleep 10	
		init 6
	fi
	
	exit 0
fi
### END ###
