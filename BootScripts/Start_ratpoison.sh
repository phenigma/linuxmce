#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting ratpoison"

export DISPLAY=:0
screen -d -m -S RatPoison /usr/pluto/bin/ratpoison
sleep 1

