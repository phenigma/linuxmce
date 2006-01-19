#!/bin/bash
. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting X server"

#screen -d -m -S XWindowSystem X -ac -allowMouseOpenFail vt7
screen -d -m -S XWindowSystem xinit /usr/pluto/bin/ratpoison -- :0 -ac -allowMouseOpenFail vt7
#sleep 1

# Disable DPMS and screen saver
#xset -display :0 -dpms s off
