#!/bin/bash

export SDL_VIDEO_X11_WMCLASS="Orbiter"

# test if XFree86 is running else manualy start it
isX11Running=$(ps -A | grep XFree86 | wc -l)
if [[ "$isX11Running" != "1" ]]; then
	/usr/pluto/bin/Start_X.sh
	export DISPLAY=:0
fi

exec ./Orbiter "$@"

