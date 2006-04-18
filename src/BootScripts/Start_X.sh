#!/bin/bash
. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting X server"

# Start X11
screen -d -m -S XWindowSystem xinit /usr/bin/icewm-session -- :0 -ac -allowMouseOpenFail vt7

# Start everouter for gyration mouse
if [[ -x /usr/pluto/bin/StartGyrationEvrouter.sh ]]; then
	screen -d -m -S GyrationMouse /usr/pluto/bin/StartGyrationEvrouter.sh
fi
