#!/bin/bash
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh

XClient=/usr/bin/icewm-session
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

# Start X11
if [[ "$Background" == y ]]; then
	screen -d -m -S XWindowSystem xinit "$XClient" "${XClientParm[@]}" -- "$XDisplay" -ac -allowMouseOpenFail "$VT" "${XServerParm[@]}"
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
	xinit "$XClient" "${XClientParm[@]}" -- "$XDisplay" -ac -allowMouseOpenFail "$VT" "${XServerParm[@]}"
fi
