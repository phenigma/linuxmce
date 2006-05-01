#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Done=0
NextStep=1
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
		1) NextStep=2 ;;
		3) NextStep=3 ;;
		*) echo "Interrupted in step '$WizStep', but shouldn't be" ;;
	esac
done
