#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

PrevVer="$2"

if [ -n "$PrevVer" ]; then
	echo "Upgrading from version '$PrevVer'. Not setting up X again"
	exit 0
else
	echo "Configuring X"
	config=$(/usr/bin/X11/X -configure 2>&1 | grep 'Your XF86Config file' | cut -d" " -f5-)
	retcode=$?
	if [ "$retcode" -ne 0 -o -z "$config" -o ! -e "$config" ]; then
		echo "Something went wrong while configuring X."
		exit 1
	fi

	[ -e /etc/X11/XF86Config-4 ] && mv /etc/X11/XF86Config-4 /etc/X11/XF86Config-4.orig
	awk '
{ print }
/Monitor.*Monitor0/ { print("\tDefaultDepth\t24") }
/Depth.*24/ { print("\t\tModes\t\"800x600\"") }
/Driver.*i810/ { print("\tOption\t\"NoAccel\"\t\"True\"") }
' "$config" >/etc/X11/XF86Config-4.new
	echo 'Section "ServerFlags"
	Option "AllowMouseOpenFail" "true"
EndSection' >>/etc/X11/XF86Config-4.new

	EnableWheel='
BEGIN {
	axis = 0;
	text = "";
	section = 0;
}

/Section "InputDevice"/,/EndSection/ {
	section = 1;
	if (match($0, /Driver.*mouse/))
		axis = 1;
	if (match($0, /ZAxisMapping/))
		axis = 0;
	if (match($0, /EndSection/) && axis == 1)
	{
		text = text "Option \"ZAxisMapping\" \"4 5\"\n";
		axis = 0;
	}
	text = text $0 "\n";
	next;
}

section == 1 {
	printf "%s", text;
	text = "";
	section = 0;
}

{ print }'

	egrep -v 'Load.*"dri"|Load.*"glx"' /etc/X11/XF86Config-4.new | awk "$EnableWheel" >/etc/X11/XF86Config-4
	rm -f "$config"

	sed -i 's!/dev/mouse!/dev/input/mice!g' /etc/X11/XF86Config-4
	# only ony standalone MDs, not hybrids
	if ! PackageIsInstalled pluto-dcerouter; then
		sed -i 's/^NTPSERVERS=.*$/NTPSERVERS="dcerouter"/' /etc/default/ntpdate
	fi
fi
