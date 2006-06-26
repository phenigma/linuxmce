#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Done=0
NextStep=$STEP_Start
export DISPLAY=:1
while [[ "$Done" -eq 0 ]]; do
	StartX -step "$NextStep"
	Ret=$(WizGet ExitCode)
	case "$Ret" in
		2) : ;; # programmed exit
		*) exit $Ret ;; # 0 = I agree; 1 = I do not agree; 3 = Skip Wizard; * = other
	esac
	WizStep=$(WizGet CurrentStep)
	Video_Ratio=$(WizGet Video_Radio)
	VideoResolution=$(WizGet VideoResolution)
	VideoRefresh=$(WizGet VideoRefresh)

	case "$WizStep" in
		$STEP_DisplayRatio) NextStep=$STEP_VideoConnector ;;
		-$STEP_DisplayRatio) NextStep=$STEP_DisplayRatio ;;
		$STEP_ScreenResolution) NextStep=$STEP_ScreenResolution ;;
		*) echo "Interrupted in step '$WizStep', but shouldn't be" ;;
	esac
done
