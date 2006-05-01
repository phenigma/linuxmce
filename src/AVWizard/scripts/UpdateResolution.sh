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
		WizSet VideoResolution '480p'
		WizSet VideoRefresh '60'
		"$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution '848x480'
		RestartX
		kill -USR1 $PPID
	;;
	resolution[+-])
		Video_Ratio=$(WizGet Video_Ratio)
		VideoResolution=$(WizGet VideoResolution)
		echo "--> Resolution cmd: $Param; Ratio: $Video_Ratio; Current resolution: $VideoResolution"
		case "${Param#resolution}" in
			+) VideoResolution_New=$(Resolution_Next "$Video_Ratio" "$VideoResolution") ;;
			-) VideoResolution_New=$(Resolution_Prev "$Video_Ratio" "$VideoResolution") ;;
		esac
		echo "--> New resolution: $VideoResolution_New"
	;;
	refresh[+-])
		VideoRefresh=$(WizGet VideoRefresh)
		echo "--> Refresh cmd: $Param; Ratio: $Video_Ratio; Current refresh: $VideoRefresh"
		case "${Param#refresh}" in
			+) VideoRefresh_New=$(Refresh_Next "$VideoRefresh") ;;
			-) VideoRefresh_New=$(Refresh_Prev "$VideoRefresh") ;;
		esac
		echo "--> New refresh: $VideoRefresh_New"
	;;
	*) echo "Unknown parameter: '$Param'"; exit 1 ;;
esac
