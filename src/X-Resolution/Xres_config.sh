#!/bin/bash

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
