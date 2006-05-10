#!/bin/bash

trap true SIGINT

Params=(Width Height Refresh Type)
ParmIdx=0

for Param in "$@"; do
	case "$Param" in
		-f) Force=--force ;;
		*)
			eval ${Params[$ParamIdx]}="$Param"
			((ParamIdx++))
		;;
	esac
done

Type="${Type:+--scantype $Type}"

ShowDialog()
{
	local pidOfX="$1"
	local Msg="$2"

	DISPLAY=:1 xsetbg -tile /usr/pluto/share/resolutions.png
	DISPLAY=:1 Xdialog --ignore-eof --cancel-label "Close" --infobox "$Msg" 0x0 0
	kill "$pidOfX"
}

cp /etc/X11/xorg.conf{,.test}
if ! /usr/pluto/bin/Xconfigure.sh --conffile /etc/X11/xorg.conf.test --resolution "${Width}x${Height}@${Refresh}" $Force $Type; then
	echo "X configuration script exited with error"
	exit 10
fi

X :1 -ac -config /etc/X11/xorg.conf.test &
pidOfX=
Timeout=5
while [[ -z "$pidOfX" && $Timeout > 0 ]]; do
	pidOfX="$(ps ax|grep 'X :1 -ac -config /etc/X11/xorg.conf.test'|grep -v grep|awk '{print $1}')"
	((Timeout--))
	sleep 1
done

if [[ -z "$pidOfX" ]]; then
	echo "X didn't start"
	exit 10
fi

setting="$(xrandr -d :1 -q|grep '^\*')"
resolution="${setting:5:11}"
refresh="${setting:39:4}"
#DISPLAY=:1 Xdialog --default-no --yesno "Current resolution: $resolution @ $refresh Hz\nIs this OK?" 0x0
Msg="Current resolution: $resolution @ $refresh Hz"
echo "$Msg"
ShowDialog "$pidOfX" "$Msg" &

disown -a
exit 0
