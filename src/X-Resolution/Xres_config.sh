#!/bin/bash

trap true SIGINT

Width="$1"
Height="$2"
Refresh="$3"
Type="$4"

Modeline="$(/usr/pluto/bin/xtiming.pl "$Width" "$Height" "$Refresh" "$Type")"
ModeName=${Modeline%% *}
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
setting="$(xrandr -d :1 -q|grep '^\*')"
resolution="${setting:5:11}"
refresh="${setting:39:4}"
DISPLAY=:1 Xdialog --default-no --yesno "Current resolution: $resolution @ $refresh Hz\nIs this OK?" 0x0
Result=$?
echo "Result: $Result"
echo "Killing test X"
kill $pidOfX
if [[ "$Result" -eq 0 ]]; then
	pidOfX=$(ps ax|grep 'X -ac -allowMouseOpenFail vt7'|grep -v grep|cut -f1 -d' ')
	mv /etc/X11/XF86Config-4{.test,}
	echo "Killing real X (to restart it with new config)"
	kill $pidOfX
	sleep 5
	/usr/pluto/bin/Start_X.sh
	/usr/pluto/bin/Start_ratpoison.sh
else
	rm /etc/X11/XF86Config-4.test
fi
