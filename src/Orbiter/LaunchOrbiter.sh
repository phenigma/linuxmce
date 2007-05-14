#!/bin/bash

## Includes
. /usr/pluto/bin/Config_Ops.sh 2>/dev/null || exit 1  ## So the orbiter can get the environment variable for the mysql server
. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null || exit 1
. /usr/pluto/bin/Utils.sh 2>/dev/null || exit 1
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

export DISPLAY=:$Display
## Detect if we're running dedicated or a shared desktop system
if [[ "$SharedDesktop" != 1 ]]; then
	### Dedicated desktop
	## Test if XFree86 is running else manualy start it
	XPID=$(</var/run/plutoX$Display.pid)
	if [[ -z "$XPID" || ! -d /proc/"$XPID" || "$(</proc/"$XPID"/cmdline)" != *"xfwm4"* ]]; then
		/usr/pluto/bin/Start_X.sh
	fi

	## Have two desktops on a dedicated desktop
	N_Desktops=2
	wmctrl -n "$N_Desktops"

	export ORBITER_PRIMARY_DESKTOP=0
	export ORBITER_SECONDARY_DESKTOP=1
else
	### Shared desktop
	AlphaBlending=$(AlphaBlendingEnabled)

	## Configure window manager trasparancy manager
	WMTweaksFile="/root/.config/xfce4/mcs_settings/wmtweaks.xml"
	if [[ "$AlphaBlending" != 1 ]]; then
		WMParm=(--compositor=off)
		WMCompTweakVal=0
	else
		WMParm=()
		WMCompTweakVal=1
	fi
	sed -i '/Xfwm\/UseCompositing/ s/value="."/value="'"$WMCompTweakVal"'"/g' "$WMTweaksFile"

	## Kill kwin, xcompmgr
	killall kwin
	killall xfwm4
	killall xcompmgr
	## Start xfwm4
	xfwm4 "${WMParm[@]}" &>/dev/null </dev/null &
	disown -a
	sleep 1
	
	N_Desktops=$(wmctrl -d | wc -l) # zero based

	Logging $TYPE $SEVERITY_NORMAL "LaunchOrbiter" "Number of desktops found: $N_Desktops ; desktops activated? $DesktopActivated"

	## Increase number of desktops by 2
	if [[ "$DesktopActivated" != 1 ]]; then
		Logging $TYPE $SEVERITY_NORMAL "LaunchOrbiter" "Increasing number of desktops to: $((N_Desktops + 2))"
		wmctrl -n $((N_Desktops + 2))
		ConfSet DesktopActivated 1
	else
		((N_Desktops -= 2))
	fi

	Logging $TYPE $SEVERITY_NORMAL "LaunchOrbiter" "Primary desktop: $N_Desktops and secondary desktop $((N_Desktops + 1))"

	## Export Orbiter desktop information variables
	export ORBITER_PRIMARY_DESKTOP=$((N_Desktops))
	export ORBITER_SECONDARY_DESKTOP=$((N_Desktops + 1))
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

if [[ "$Valgrind" == *"$Executable"* ]]; then
	$VGcmd "$Executable" "$@"
	Orbiter_RetCode=$?
else
	"$Executable" "$@"
	Orbiter_RetCode=$?
fi

## Restore number of desktops
if [[ "$SharedDesktop" == 1 && "$DesktopActivated" == 1 ]]; then

	N_Desktops=$(wmctrl -d | wc -l) # zero based
	wmctrl -n $((N_Desktops - 2))
	ConfSet DesktopActivated 0

        Logging $TYPE $SEVERITY_NORMAL "LaunchOrbiter" "Decreasing number of desktops to $((N_Desktops - 2))"
fi

exit ${Orbiter_RetCode}
