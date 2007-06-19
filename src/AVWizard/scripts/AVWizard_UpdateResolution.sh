#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Param="$1"
case "$Param" in
	reset)
		VideoResolution=$(WizGet VideoResolution)
		VideoRefresh=$(WizGet VideoRefresh)
		echo "--> Reset resolution to default (was resolution: $VideoResolution@$VideoRefresh)"
		if [[ "$VideoRefresh" == 60 && "$VideoResolution" == 640x480 ]]; then
			exit 0 # No change required
		fi
		killall -USR2 AVWizard
		bash -x "$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution '640x480' | tee-pluto /var/log/pluto/Xconfigure.log
		echo '640x480 60 640 480 VGA' >/tmp/avwizard-resolution.txt
	;;
	set_resolution)
		set -x
		RequestedResolution="$2"
		RequestedRefresh="$3"
		RequestedConnector="${4:-VGA}"
		echo "--> Setting resolution: $RequestedResolution@$RequestedRefresh"
		ResolutionFullName=$(Resolution_GetFullName "$RequestedResolution")

		if [[ "$ResolutionFullName" == *=* ]]; then
			VideoResolution_Name="${ResolutionFullName%%=*}"
			VideoResolution_Size="${ResolutionFullName#*=}"
		else
			VideoResolution_Name="$ResolutionFullName"
			VideoResolution_Size="$ResolutionFullName"
		fi

		killall -USR1 AVWizard

		bash -x "$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution "$VideoResolution_Size@$RequestedRefresh" --output "$RequestedConnector" | tee-pluto /var/log/pluto/Xconfigure.log
		WindowWidth="${VideoResolution_Size%x*}"
		WindowHeight="${VideoResolution_Size#*x}"
		echo "$VideoResolution_Name $RequestedRefresh $WindowWidth $WindowHeight $RequestedConnector" >/tmp/avwizard-resolution.txt
		echo "--> Finished setting resolution"
		set +x
	;;
	*) echo "Unknown parameter: '$Param'"; exit 1 ;;
esac
