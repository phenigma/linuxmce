#!/bin/bash

# unset variables of interest
# it happened that the script failed to do its task because the variable seemed to already be present in the environment
unset Display

. /usr/pluto/bin/Config_Ops.sh

if [[ "$Display" == "" ]] ;then
	ConfSet "Display" "0"
fi

# Run Xconfigure on upgrade to ensure that xorg.conf is correct
/usr/pluto/bin/Xconfigure.sh
