#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

if [[ "$Display" == "" ]] ;then
	ConfSet "Display" "0"
fi
