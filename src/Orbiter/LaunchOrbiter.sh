#!/bin/bash

export SDL_VIDEO_X11_WMCLASS="Orbiter"

Executable=./Orbiter
OrbiterConf=/etc/Orbiter.conf

# TODO: update Config_Ops.sh to support user-specified files, and use that here
if grep -q "UseOpenGL.*=,*1" "$OrbiterConf"; then
	Executable=./OrbiterGL
fi

# test if XFree86 is running else manualy start it
isX11Running=$(ps -A | grep Xorg | wc -l)
if [[ "$isX11Running" != "1" ]]; then
	/usr/pluto/bin/Start_X.sh
	export DISPLAY=:0
fi

xset m 1 1
exec "$Executable" "$@"
