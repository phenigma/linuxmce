#!/bin/bash
# flickr control script
#
# DESCRIPTION:
#  This script will control the flickr.pl
#  TRIVIAL IMPLEMENTATION, PLEASE CORRECT!
#
# USAGE :
#  flickr_status.sh [ -enable | -disable | -status ]
#

EnableFlagFile=/etc/pluto/flickr-enabled

case "$1" in
	-enable) # enable option
		echo 1 >"$EnableFlagFile"
		screen -d -m -S flickr /usr/pluto/bin/flickr.pl
	;;
	-disable) # disable option
		echo 0 >"$EnableFlagFile"
		killall flickr.pl
	;;
	-status) # status option
		if [[ -f "$EnableFlagFile" ]]; then
			Status=$(<"$EnableFlagFile")
		else
			Status=1
		fi
		echo "$Status"
	;;
	*) # wrong param
		echo "USAGE : flickr_status.sh [ -enable | -disable | -status ]"
	;;
esac
