. /usr/pluto/bin/Config_Ops.sh

echo "DEBUG: Last config'ed version: '$2'"

if [ ! -z "$X_Was_Set" ]; then
	echo "Configuration file says this is an upgrade. Not setting up X twice"
	exit 0
fi

echo "Configuring X"
config=$(/usr/bin/X11/X -configure 2>&1 | grep 'Your XF86Config file' | cut -d" " -f5-)
retcode=$?
if [ "$retcode" -ne 0 -o -z "$config" -o ! -e "$config" ]; then
	echo "Something went wrong while configuring X."
	exit 1
fi
ConfSet X_Was_Set true

[ -e /etc/X11/XF86Config-4 ] && mv /etc/X11/XF86Config-4 /etc/X11/XF86Config-4.orig
awk '
{ print }
/Monitor.*Monitor0/ { print("\tDefaultDepth\t16") }
/Depth.*16/ { print("\t\tModes\t\"800x600\"") }
' "$config" >/etc/X11/XF86Config-4
rm -f "$config"

[ -e /dev/mouse ] || ln -s /dev/psaux /dev/mouse
