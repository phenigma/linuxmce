#!/bin/bash
. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting X server"

screen -d -m -S XWindowSystem X -ac -allowMouseOpenFail vt7
sleep 5

