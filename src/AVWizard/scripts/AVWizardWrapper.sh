#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

SpcField()
{
	local Field="$1" Data="$2"

	echo "$Data" | cut -d' ' -f"$Field"
}

Done=0
NextStep=$STEP_Welcome
export DISPLAY=:1
while [[ "$Done" -eq 0 ]]; do
	echo "Running Wizard from step '$NextStep'"
	StartX -step "$NextStep"
	Ret=$(WizGet ExitCode)
	case "$Ret" in
		2) : ;; # programmed exit
		*) exit $Ret ;; # 0 = I agree; 1 = I do not agree; 3 = Skip Wizard; * = other
	esac
	WizStep=$(WizGet InterruptedStep)

	if [[ -f /tmp/avwizard-resolution.txt ]]; then
		WizResolution=$(< /tmp/avwizard-resolution.txt)
		Video_Ratio=$(SpcField 1 "$WizResolution")
		VideoResolution_Name=$(SpcField 2 "$WizResolution")
		VideoRefresh=$(SpcField 3 "$WizResolution")
		WindowWidth=$(SpcField 4 "$WizResolution")
		WindowHeight=$(SpcField 5 "$WizResolution")

		set -x
		WizSet Video_Ratio "$Video_Ratio"
		WizSet VideoResolution "$VideoResolution_Name"
		WizSet VideoRefresh "$VideoRefresh"
		WizSet WindowWidth "$WindowWidth"
		WizSet WindowHeight "$WindowHeight"
		set +x

		rm -f /tmp/avwizard-resolution.txt
	fi

	echo "Interrupted in step '$WizStep'"

	case "$WizStep" in
		$STEP_VideoRatio) NextStep=$STEP_VideoOutput ;;
		-$STEP_VideoRatio) NextStep=$STEP_VideoRatio ;;
		$STEP_VideoResolution) NextStep=$STEP_VideoResolution ;;
		*) echo "Interrupted in step '$WizStep', but shouldn't be"; NextStep=$WizStep ;;
	esac
done
