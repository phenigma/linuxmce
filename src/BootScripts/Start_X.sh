#!/bin/bash
. /usr/pluto/bin/pluto.func

XClient=/usr/bin/icewm-session
XClientParm=()
Background=y

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		-client) ((i++)); XClient=${!i} ;;
		-parm) ((i++)); XClientParm=("${XClientParm[@]}" ${!i}) ;;
		-fg) Background=n
	esac
done

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting X server (client: $XClient; parms: ${XClientParm[*]})"

# Start X11
if [[ "$Background" == y ]]; then
	screen -d -m -S XWindowSystem xinit "$XClient" "${XClientParm[@]}" -- :0 -ac -allowMouseOpenFail vt7
	# Start everouter for gyration mouse
	if [[ -x /usr/pluto/bin/StartGyrationEvrouter.sh ]]; then
		screen -d -m -S GyrationMouse /usr/pluto/bin/StartGyrationEvrouter.sh
	fi
else
	xinit "$XClient" "${XClientParm[@]}" -- :0 -ac -allowMouseOpenFail vt7
fi
