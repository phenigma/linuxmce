#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

STEP_Start=1
STEP_DisplayRatio=2
STEP_VideoConnector=3
STEP_ScreenResolution=4
STEP_AudioVolume=5
STEP_AudioAC3=6
STEP_AudioDTS=7
STEP_Final=8

Done=0
NextStep=0
export DISPLAY=:0
while [[ "$Done" -eq 0 ]]; do
	StartX -step "$NextStep"
	Ret=$(WizGet ExitCode)
	case "$Ret" in
		0|1|255) exit $Ret ;;
	esac
	WizStep=$(WizGet CurrentStep)
	Video_Ratio=$(WizGet Video_Radio)
	VideoResolution=$(WizGet VideoResolution)
	VideoRefresh=$(WizGet VideoRefresh)

	case "$WizStep" in
		$STEP_DisplayRatio) NextStep=$STEP_VideoConnector ;;
		$STEP_ScreenResolution) NextStep=$STEP_ScreenResolution ;;
		*) echo "Interrupted in step '$WizStep', but shouldn't be" ;;
	esac
done
