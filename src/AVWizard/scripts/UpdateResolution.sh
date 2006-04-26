#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Param="$1"
case "$Param" in
	4[_:]3)
		exit 0; # Already in 4:3; no change
	;;
	16[_:]9)
		$Wiz -set Video_Radio '16_9'
		$Wiz -set Video_Resolution '480p'
		$Wiz -set Video_Refresh '60'
		"$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution '848x480'
		kill -USR1 $PPID
	;;
	resolution[+-]|refresh[+-]) ;;
	*) echo "Unknown parameter: '$Param'"; exit 1 ;;
esac
