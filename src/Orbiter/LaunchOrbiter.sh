#!/bin/bash

## Includes
. /usr/pluto/bin/Config_Ops.sh 2>/dev/null || exit 1  ## So the orbiter can get the environment variable for the mysql server
. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null || exit 1
. /usr/pluto/bin/pluto.func 2>/dev/null || exit 1

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
XPID=$(</var/run/plutoX$Display.pid)
if [[ -z "$XPID" || ! -d /proc/"$XPID" || "$(</proc/"$XPID"/cmdline)" != *"xfwm4"* ]]; then
	/usr/pluto/bin/Start_X.sh
	export DISPLAY=:$Display
fi

Counter=0
isIceWmRunning=0
while [[ "$isIceWmRunning" -eq 0 ]]; do
	isIceWmRunning=$(ps -A | egrep 'xfwm4|icewm' | wc -l)
	if [[ "$isIceWmRunning" -eq 0 ]]; then
		((Counter++))
		if [[ "$Counter" -gt 20 ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "LaunchOrbiter" "xfwm4 failed to start"
			break
		fi
		sleep 1
	fi
done


## Run Orbiter
xset m 1 1

## Nasty hack ...  It seems icewm somehow ignores the first window that starts, and when we didn't start xterm first
## then Orbiter would start, launch Xine, and when you went to play a movie, xine would be full screen and topmost
## and Orbiter couldn't go on top of xine
#Logging "$TYPE" "$SEVERITY_CRITICAL" "LaunchOrbiter" "Hack to start xterm"
#killall xterm
#xterm -class bogus_xterm -name bogus_xterm -geometry 1x1+2000+2000 &
#sleep 1
if [[ "$Valgrind" == *"$Executable"* ]]; then
	exec $VGcmd "$Executable" "$@"
else
	exec "$Executable" "$@"
fi
#sleep 5
#Logging "$TYPE" "$SEVERITY_CRITICAL" "LaunchOrbiter" "Hack to kill xterm"
#killall xterm
