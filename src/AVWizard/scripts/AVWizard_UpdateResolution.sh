#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Param="$1"
case "$Param" in
	reset)
		Video_Ratio=$(WizGet Video_Ratio)
		VideoResolution=$(WizGet VideoResolution)
		echo "--> Reset resolution to default (was $Video_Ratio; resolution: $VideoResolution)"
		if [[ "$Video_Ratio" == 4_3 && "$VideoResolution" == 640x480 ]]; then
			exit 0 # No change required
		fi
		kill -USR2 $PPID
		"$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution '640x480'
		echo '4_3 640x480 60 640 480' >/tmp/avwizard-resolution.txt
	;;
	4[_:]3)
		Video_Ratio=$(WizGet Video_Ratio)
		VideoResolution=$(WizGet VideoResolution)
		echo "--> Aspect ratio 4:3 (was $Video_Ratio; resolution: $VideoResolution)"
		if [[ "$Video_Ratio" == 4_3 && "$VideoResolution" == 640x480 ]]; then
			exit 0 # No change required
		fi
		kill -USR1 $PPID
		"$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution '640x480'
		echo '4_3 640x480 60 640 480' >/tmp/avwizard-resolution.txt
	;;
	16[_:]9)
		Video_Ratio=$(WizGet Video_Ratio)
		VideoResolution=$(WizGet VideoResolution)
		echo "--> Aspect ratio 16:9 (was $Video_Ratio; resolution: $VideoResolution)"
		if [[ "$Video_Ratio" == 16_9 ]]; then
			exit 0 # No change required
		fi
		kill -USR1 $PPID
		Video_Ratio=$(WizGet Video_Ratio)
		"$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution '848x480'
		echo '16_9 480p 60 848 480' >/tmp/avwizard-resolution.txt
	;;
	resolution[+-]|refresh[+-])
		Video_Ratio=$(WizGet Video_Ratio)
		VideoResolution=$(WizGet VideoResolution)
		VideoRefresh=$(WizGet VideoRefresh)
		echo "--> Cmd: $Param; Ratio: $Video_Ratio; Current resolution: $VideoResolution@$VideoRefresh"
		case "${Param#resolution}" in
			+) VideoResolution_New=$(Resolution_Next "$Video_Ratio" "$VideoResolution") ;;
			-) VideoResolution_New=$(Resolution_Prev "$Video_Ratio" "$VideoResolution") ;;
		esac
		case "${Param#refresh}" in
			+) VideoRefresh_New=$(Refresh_Next "$VideoRefresh") ;;
			-) VideoRefresh_New=$(Refresh_Prev "$VideoRefresh") ;;
		esac

		if [[ -z "$VideoResolution_New" ]]; then
			VideoResolution_New="$(Resolution_GetFullName "$Video_Ratio" "$VideoResolution")"
		fi
		if [[ -z "$VideoRefresh_New" ]]; then
			VideoRefresh_New="$VideoRefresh"
		fi

		echo "--> New resolution: $VideoResolution_New (was: $VideoResolution)"
		echo "--> New refresh: $VideoRefresh_New (was: $VideoRefresh)"

		if [[ "$VideoResolution_New" == *=* ]]; then
			VideoResolution_Name="${VideoResolution_New%%=*}"
			VideoResolution_Size="${VideoResolution_New#*=}"
		else
			VideoResolution_Name="$VideoResolution_New"
			VideoResolution_Size="$VideoResolution_New"
		fi

		echo [[ "$VideoResolution" "$VideoResolution_Name" "$VideoRefresh" "$VideoRefresh_New" ]]
		if [[ "$VideoResolution" == "$VideoResolution_Name" && "$VideoRefresh" == "$VideoRefresh_New" ]]; then
			exit 0 # No change required
		fi
		kill -USR1 $PPID

		"$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution "$VideoResolution_Size@$VideoRefresh_New"
		WindowWidth="${VideoResolution_Size%x*}"
		WindowHeight="${VideoResolution_Size#*x}"
		echo "$Video_Ratio $VideoResolution_Name $VideoRefresh_New $WindowWidth $WindowHeight" >/tmp/avwizard-resolution.txt
	;;
	*) echo "Unknown parameter: '$Param'"; exit 1 ;;
esac
