#!/bin/bash

# unset variables of interest
# it happened that the script failed to do its task because the variable seemed to already be present in the environment
unset LogLevels

. /usr/pluto/bin/Config_Ops.sh

if [[ "$LogLevels" == "" ]] ;then
	ConfSet "LogLevels" "1,5,7,8"
fi
