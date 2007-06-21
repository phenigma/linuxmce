#!/bin/bash

if [[ -x /usr/pluto/bin/shift_state ]] ;then
	if [[ "$( /usr/pluto/bin/shift_state )" == "1" ]] ;then
		. /usr/pluto/bin/Config_Ops.sh 
		ConfSet "AVWizardOverride" "1"
	fi
fi
