#!/bin/bash

SRA=/usr/pluto/bin/SetupRemoteAccess.sh
Cfg=/usr/pluto/bin/Config_Ops.sh
if [[ ! -x "$Cfg" ]]; then
	echo "Needed script was not found."
	echo "You installation probably terminated in a very early stage"
	exit 1
fi

. "$Cfg"

Device="$PK_Device"
Code="$Activation_Code"
Installation="$PK_Installation"

echo "This script will tunnel the SSH and Web ports to our support server"

[[ -z "$Installation" ]] && State=0 || State=2
[[ -n "$Device" && -n "$Code" && "$State" -eq 0 ]] && State=1

while [[ "$State" -lt 2 ]]; do
	if [[ "$State" -eq 0 ]]; then
		echo -n "Device number: "; read Device
		echo -n "Activation code: "; read Code
	fi

	Installation=$(wget -O - "http://activate.plutohome.com/get_installation.php?device=$Device&code=$Code" 2>/dev/null)
	if [[ "$?" -ne 0 ]]; then
		echo "Failed to get installation number from plutohome.com"
		echo "Exiting"
		exit 1
	fi
	[[ -n "$Installation" ]] && State=2 || State=0

	if [[ "$State" -eq 0 ]]; then
		echo "Error. Device or Activation code probably incorrect."
		echo -n "Try again? [Y/n]: "; read Answer
		if [[ -n "$Answer" && "$Answer" != "y" && "$Answer" != "Y" ]]; then
			State=3
		fi
	fi
done

[[ "$State" -eq 3 ]] && exit

echo "Installation number: $Installation"
echo -n "Type in a password for remote access: "
/bin/stty -echo; read Password; /bin/stty echo
ConfSet remote "$Password"
ConfSet PK_Installation "$Installation"
"$SRA"
