#!/bin/bash

## Includes
. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null || exit 1

## Read command line params
Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
                -d) ((i++)); DeviceID="${Params[$i]}" ;;
        esac
done

## SDL Exports 
export SDL_VIDEO_X11_WMCLASS="Orbiter"
export SDL_MOUSE_RELATIVE=0

## Check to see if we use OpenGL efects or not
DD_USE_OPENGL_EFECTS=172
Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$DeviceID' AND FK_DeviceData=${DD_USE_OPENGL_EFECTS}"
UseOpenGL=$(RunSQL "$Q")

Executable=./Orbiter
if [[ "$UseOpenGL" == "1"  ]]; then
	Executable=./OrbiterGL
fi

## Test if XFree86 is running else manualy start it
isX11Running=$(ps -A | grep Xorg | wc -l)
if [[ "$isX11Running" != "1" ]]; then
	/usr/pluto/bin/Start_X.sh
	export DISPLAY=:0
fi

## Run Orbiter
xset m 1 1
exec "$Executable" "$@"
