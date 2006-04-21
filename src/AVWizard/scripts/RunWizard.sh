#!/bin/bash

BaseDir=/usr/pluto/bin
BaseDir=.

Wiz="$BaseDir"/AVWizard
AudioMixerSettingsAwk="$BaseDir"/AudioMixerSettings.awk

Wiz=echo

GetAudioMixerSettings()
{
	local Settings
	
	Settings="$(amixer sget Master | awk -f "$AudioMixerSettingsAwk")"
	eval "$Settings"
}

Done=0
while [[ "$Done" -eq 0 ]]; do
	$Wiz -set Video_Ratio 4_3
	$Wiz -set Video_Resolution 640x480
	$Wiz -set Video_Refresh 60
	"$BaseDir"/Xconfigure.sh --defaults --resolution '640x480@60'
	"$BaseDir"/Start_X.sh
	$Wiz -set Video_Output VGA
	$Wiz -set AudioConnector 'Analog Stereo'
	GetAudioMixerSettings
	$Wiz -set AudioVolumeMin $AudioVolumeMin
	$Wiz -set AudioVolumeMax $AudioVolumeMax
	$Wiz -set AudioVolumeCurrent $AudioVolumeCurrent
	$Wiz -set AudioVolumeIncrement 1
	$Wiz -set DolbyTest 0
	$Wiz -set DTSTest 0

	if "$BaseDir"/AVWizardWrapper.sh; then
		Done=1
	fi
done
