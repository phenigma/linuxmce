#!/bin/bash

trap true SIGINT

Width="$1"
Height="$2"
Refresh="$3"
Type="$4"

ShowDialog()
{
	local pidOfX="$1"
	local Msg="$2"

	DISPLAY=:1 Xdialog --ignore-eof --cancel-label "Close" --infobox "$Msg" 0x0 0
	kill "$pidOfX"
}

Modeline="$(/usr/pluto/bin/xtiming.pl "$Width" "$Height" "$Refresh" "$Type")"
if [[ "$Refresh" != "60" ]]; then
	ModeName="${Modeline%% *}"
else
	# Workaround of the 'i810' driver which decides on its own to ignore ModeLines completely
	ModeName="\"${Width}x${Height}\""
fi

awk '
	BEGIN { Monitor = 0; Display = 0; }
	/Modeline/ || /Modes/ { next }
	/Section..*"Monitor"/ { print; Monitor = 1; next; }
	/EndSection/ && Monitor == 1 { print "\tModeline '"${Modeline//\"/\\\"}"'"; print; Monitor = 0; next; }
	/SubSection..*"Display"/ { print; Display = 1; next; }
	/EndSubSection/ && Display == 1 { print "\t\tModes '"${ModeName//\"/\\\"}"'"; print; Display = 0; next; }
	{ print }
' /etc/X11/XF86Config-4 >/etc/X11/XF86Config-4.test

X :1 -ac -xf86config /etc/X11/XF86Config-4.test &
sleep 1
pidOfX="$(ps ax|grep 'X :1 -ac -xf86config /etc/X11/XF86Config-4.test'|grep -v grep|cut -f1 -d' ')"
if [[ -z "$pidOfX" ]]; then
	echo "X didn't start"
	exit 1
fi

setting="$(xrandr -d :1 -q|grep '^\*')"
resolution="${setting:5:11}"
refresh="${setting:39:4}"
#DISPLAY=:1 Xdialog --default-no --yesno "Current resolution: $resolution @ $refresh Hz\nIs this OK?" 0x0
Msg="Current resolution: $resolution @ $refresh Hz"
echo "$Msg"
ShowDialog "$pidOfX" "$Msg" &

disown -ah
exit 0
