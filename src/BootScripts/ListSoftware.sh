#!/bin/bash

state="$1"
IP="$2"

[ "$state" == "install" ] && state="ii"

/usr/pluto/bin/LaunchRemoteCmd.sh "$IP" "dpkg -l | grep -F '$state' | awk '{print \$2\" = \"\$3}'"
