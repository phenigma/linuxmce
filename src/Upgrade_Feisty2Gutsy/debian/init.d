#!/bin/bash

case "$1" in
	start)
		# Stop Usplash
		/etc/init.d/usplash start
		sleep 3

		echo "We're here bastard ..."
		sleep 20

		#
		;;
	*)
		echo "Usage: $0 {start}" >&2
		exit 2
		;;
esac
exit 0

