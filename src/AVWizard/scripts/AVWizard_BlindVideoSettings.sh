#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Output=VGA
Resolution=640x480

ParseParms()
{
	local i
	for ((i = 1; i <= "$#"; i++)); do
		case "${!i}" in
			--resolution) ((i++)); Resolution="${!i}" ;;
			--output) ((i++)); Output="${!i}" ;;
		esac
	done
}

ParseParms "$@"
/usr/pluto/bin/AVWizard_UpdateResolution.sh set_resolution "$Resolution" 60 "$Output"
