#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Done=0
NextStep=1
while [[ "$Done" -eq 0 ]]; do
	$Wiz -step "$NextStep"
	Ret=$?
	case "$Ret" in
		0|1) exit $Ret ;;
	esac
	WizStep=$(WizGet CurrentStep)
	Video_Ratio=$(WizGet Video_Radio)
	Video_Resolution=$(WizGet Video_Resolution)
	Video_Refresh=$(WizGet Video_Refresh)

	case "$WizStep" in
		1) NextStep=2 ;;
	esac
done
