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
/Monitor.*Monitor0/ { print("\tDefaultDepth\t16") }
/Depth.*16/ { print("\t\tModes\t\"800x600\"") }
' "$config" >/etc/X11/XF86Config-4
	rm -f "$config"

	sed -i 's!/dev/mouse!/dev/input/mice!g' /etc/X11/XF86Config-4
fi
