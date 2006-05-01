#!/bin/bash
. /usr/pluto/bin/pluto.func

XClient=/usr/bin/icewm-session

for ((i = 1; i < "$#"; i++)); do
	case "${!i}" in
		--client) ((i++)); XClient=${!i} ;;
	esac
done

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting X server (client: $XClient)"

# Start X11
screen -d -m -S XWindowSystem xinit "$XClient" -- :0 -ac -allowMouseOpenFail vt7

# Start everouter for gyration mouse
if [[ -x /usr/pluto/bin/StartGyrationEvrouter.sh ]]; then
	screen -d -m -S GyrationMouse /usr/pluto/bin/StartGyrationEvrouter.sh
fi
