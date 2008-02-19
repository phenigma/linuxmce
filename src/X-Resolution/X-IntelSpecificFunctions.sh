#!/bin/bash

XvMC_Lib()
{
	echo "libI810XvMC.so.1"
}

SetResolution()
{
	local Resolution="$1" ResX="$2" ResY="$3" Refresh="$4" ScanType="$5" Output="$6" TVStandard="$7"
	local TVType TVOutput
	local AvailableConnectors="VGA TMDS-1" EnabledConnector

	case "$TVStandard" in
		HD480p) TVType=480p ;;
		HD720p) TVType=720p ;;
		HD1080i) TVType=1080i ;;
		PAL*) TVType=PAL ;;
		NTSC*) TVType=NTSC ;;
	esac

	case "$Output" in
		#Component) : ;;
		#Composite) : ;;
		#S-Video) : ;;
		VGA) EnabledConnector="VGA" ;;
		DVI) EnabledConnector="TMDS-1" ;;
		*) EnabledConnector="VGA" ;;
	esac
	
	Modeline="$(GenModeline "$DisplayDriver" "$ResX" "$ResY" "$Refresh" "$ScanType")"
	
	awk -v"ResX=$ResX" -v"ResY=$ResY" -v"Refresh=$Refresh" -v"Modeline=$Modeline" -v"nvHD=$TVStandard" -v"ConnectedMonitor=$ConnectedMonitor" -v"TVOutFormat=$TVOutFormat" -f/usr/pluto/bin/X-ChangeResolution.awk "$ConfigFile" >"$ConfigFile.$$"
	mv "$ConfigFile"{.$$,}

	(
		echo "#!/bin/bash"
		echo "for Conn in $AvailableConnectors; do"
		echo '	/usr/bin/xrandr -d $DISPLAY --output $Conn --off'
		echo "done"
		echo "/usr/bin/xrandr -d \$DISPLAY --output $EnabledConnector --auto"
		echo "/usr/bin/xrandr -d \$DISPLAY -s ${ResX}x${ResY}"
	) >/etc/pluto/X-PostStart.sh
	chmod +x /etc/pluto/X-PostStart.sh
}
