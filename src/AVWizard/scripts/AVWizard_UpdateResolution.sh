#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Param="$1"
case "$Param" in
	reset)
		VideoResolution=$(WizGet VideoResolution)
		VideoRefresh=$(WizGet VideoRefresh)
		VideoOutput=$(WizGet VideoOutput)

		ResolutionDefaults="640x480 60 640 480 VGA"
		if [[ -f "/tmp/avwizard-resolution-defaults.txt" ]]; then
			ResolutionDefaults="$(</tmp/avwizard-resolution-defaults.txt)"
		fi

		DefaultResolution=$(SpcField 1 "$ResolutionDefaults")
		DefaultRefresh=$(SpcField 2 "$ResolutionDefaults")
		DefaultConnector=$(SpcField 5 "$ResolutionDefaults")

		DefaultResolutionFullName=$(Resolution_GetFullName "$DefaultResolution")
		if [[ "$DefaultResolutionFullName" == *=* ]]; then
			DefaultResolution_Name="${DefaultResolutionFullName%%=*}"
			DefaultResolution_Size="${DefaultResolutionFullName#*=}"
		else
			DefaultResolution_Name="$DefaultResolutionFullName"
			DefaultResolution_Size="$DefaultResolutionFullName"
		fi

		echo "--> Reset resolution to default: $DefaultResolution@$DefaultRefresh/$DefaultConnector (was resolution: $VideoResolution@$VideoRefresh/$VideoOutput)"
		if [[ "$VideoRefresh" == "$DefaultRefresh" && "$VideoResolution" == "$DefaultResolution" && "$VideoOutput" == "$DefaultConnector" ]]; then
			exit 0 # No change required
		fi
		killall -USR2 AVWizard
		bash -x "$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution "$DefaultResolution_Size" --output "$DefaultConnector" | tee-pluto /var/log/pluto/Xconfigure.log
		echo "$ResolutionDefaults" >/tmp/avwizard-resolution.txt
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
