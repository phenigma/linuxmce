#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

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
	Video_Ratio=$(WizGet Video_Radio)
	VideoResolution=$(WizGet VideoResolution)
	VideoRefresh=$(WizGet VideoRefresh)

	echo "Interrupted in step '$WizStep'"
	case "$WizStep" in
		$STEP_VideoRatio) NextStep=$STEP_VideoOutput ;;
		-$STEP_VideoRatio) NextStep=$STEP_VideoRatio ;;
		$STEP_VideoResolution) NextStep=$STEP_VideoResolution ;;
		*) echo "Interrupted in step '$WizStep', but shouldn't be"; NextStep=$WizStep ;;
	esac
done
