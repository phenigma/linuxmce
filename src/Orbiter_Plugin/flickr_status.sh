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

mkdir -p /etc/pluto

Status()
{
		if [[ -f "$EnableFlagFile" ]]; then
			Status=$(<"$EnableFlagFile")
		else
			Status=1
		fi
		echo "$Status"
}

case "$1" in
	-enable) # enable option
		if [[ "$(Status)" == "1" ]]; then
			exit #already enabled
		fi
		echo 1 >"$EnableFlagFile"
		screen -d -m -S flickr /usr/pluto/bin/flickr.pl
	;;
	-disable) # disable option
		if [[ "$(Status)" != "1" ]]; then
			exit #already disabled
		fi
		echo 0 >"$EnableFlagFile"
		killall flickr.pl 2>/dev/null
	;;
	-status) # status option
		Status
	;;
	*) # wrong param
		echo "USAGE : flickr_status.sh [ -enable | -disable | -status ]"
	;;
esac
