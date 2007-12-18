#!/bin/bash

case "$1" in
	start)
		# Stop Usplash
		/etc/init.d/usplash start
		chvt 8
		sleep 3

		echo "Trying to update the system ..."
		/usr/share/gutsy-upgrade-scripts/scripts/upgrade-apply.sh

		echo "Press <ENTER> to continue ....."
		read 
		#
		;;
	*)
		echo "Usage: $0 {start}" >&2
		exit 2
		;;
esac
exit 0

