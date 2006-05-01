#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Param="$1"
case "$Param" in
	4[_:]3)
		echo "--> Aspect ratio 4:3"
		exit 0; # Already in 4:3; no change
	;;
	16[_:]9)
		echo "--> Aspect ratio 16:9"
		WizSet Video_Ratio '16_9'
		WizSet Video_Resolution '480p'
		WizSet Video_Refresh '60'
		"$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution '848x480'
		RestartX
		kill -USR1 $PPID
	;;
	resolution[+-])
		Video_Ratio=$(WizGet Video_Ratio)
		Video_Resolution=$(WizGet Video_Resolution)
		echo "--> Resolution cmd: $Param; Ratio: $Video_Ratio; Current resolution: $Video_Resolution"
		case "${Param#resolution}" in
			+) Video_Resolution_New=$(Resolution_Next "$Video_Ratio" "$Video_Resolution") ;;
			-) Video_Resolution_New=$(Resolution_Prev "$Video_Ratio" "$Video_Resolution") ;;
		esac
		echo "--> New resolution: $Video_Resolution_New"
	;;
	refresh[+-])
		Video_Refresh=$(WizGet Video_Refresh)
		echo "--> Refresh cmd: $Param; Ratio: $Video_Ratio; Current refresh: $Video_Refresh"
		case "${Param#refresh}" in
			+) Video_Refresh_New=$(Refresh_Next "$Video_Refresh") ;;
			-) Video_Refresh_New=$(Refresh_Prev "$Video_Refresh") ;;
		esac
		echo "--> New refresh: $Video_Refresh_New"
	;;
	*) echo "Unknown parameter: '$Param'"; exit 1 ;;
esac
