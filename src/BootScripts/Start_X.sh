#!/bin/bash
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh

function assureXorgSane() {

	xorgLines=$(cat /etc/X11/xorg.conf | wc -l)
	if [ $xorgLines -le 15 ] ;then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "File xorg.conf has only $xorgLines lines, rebuilding"
		EVICETEMPLATE_OnScreen_Orbiter=62
		DEVICETEMPLATE_OrbiterPlugin=12
		DEVICECATEGORY_Media_Director=8
		DEVICEDATA_ScreenWidth=100
		DEVICEDATA_ScreenHeight=101
		DEVICEDATA_PK_Size=25
		DEVICEDATA_Video_settings=89
		DEVICEDATA_Spacing=150
		DEVICEDATA_Offset=167

		. /usr/pluto/bin/Utils.sh
		. /usr/pluto/bin/SQL_Ops.sh

		ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')

		Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings' LIMIT 1"
		VideoSetting=$(RunSQL "$Q")
		VideoSetting=$(Field "1" "$VideoSetting")
		
		refresh=$(echo $VideoSetting | cut -d '/' -f2)
		resolution=$(echo $VideoSetting | cut -d '/' -f1)
		width=$(echo $resolution | cut -d' ' -f1)
		height=$(echo $resolution | cut -d' ' -f2)

		/usr/pluto/bin/Xconfigure.sh --defaults --resolution "${width}x${height}@${refresh}"
	fi
}

XClient=/usr/pluto/bin/Start_IceWM.sh
XClientParm=()
XServerParm=(-logverbose 5)
Background=y
XDisplay=:0

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		-client) ((i++)); XClient=${!i} ;;
		-parm) ((i++)); XClientParm=("${XClientParm[@]}" ${!i}) ;;
		-fg) Background=n ;;
		-srvparm) ((i++)); XServerParm=("${XServerParm[@]}" ${!i}) ;;
		-display) ((i++)); XDisplay=${!i} ;;
	esac
done

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting X server (client: $XClient; parms: ${XClientParm[*]})"

VT=${XDisplay#:}
VT=vt"$((7+VT))"

/usr/pluto/bin/SoundCards_Setup.sh
/usr/pluto/bin/CaptureCards_Setup.sh

assureXorgSane
assureXorgSane

# Start X11
if [[ "$Background" == y ]]; then
	screen -d -m -S XWindowSystem xinit "$XClient" "${XClientParm[@]}" -- "$XDisplay" -ignoreABI -ac -allowMouseOpenFail "$VT" "${XServerParm[@]}"
	# Start everouter for gyration mouse
	#if [[ -x /usr/pluto/bin/StartGyrationEvrouter.sh ]]; then
	#	screen -d -m -S GyrationMouse /usr/pluto/bin/StartGyrationEvrouter.sh
	#fi
	sleep 1

	AlphaBlending=$(AlphaBlendingEnabled)
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "X server: backround; AlphaBlending: $AlphaBlending"
	if [[ "$AlphaBlending" == 1 ]]; then
		DISPLAY=:0 /usr/bin/xcompmgr &
		disown -a
	fi
else
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "X server: foreground"
	xinit "$XClient" "${XClientParm[@]}" -- "$XDisplay" -ac -ignoreABI -allowMouseOpenFail "$VT" "${XServerParm[@]}"
fi
