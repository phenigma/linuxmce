#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Done=0
while [[ "$Done" -eq 0 ]]; do
	$Wiz
	Ret=$?
	case "$Ret" in
		0|1) exit $Ret ;;
	esac
	WizStep=$(WizGet CurrentStep)
	Video_Ratio=$(WizGet Video_Radio)
	Video_Resolution=$(WizGet Video_Resolution)
	Video_Refresh=$(WizGet Video_Refresh)

	case "$WizStep" in
		1) "$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution "$Video_Resolution@$Video_Refresh" ;;
	esac
done
