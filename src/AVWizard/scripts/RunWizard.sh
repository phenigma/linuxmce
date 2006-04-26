#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

AudioMixerSettingsAwk="$BaseDir"/AudioMixerSettings.awk

GetAudioMixerSettings()
{
	local Settings
	
	Settings="$(amixer sget Master | awk -f "$AudioMixerSettingsAwk")"
	eval "$Settings"
}

CleanUp()
{
	rm -f /tmp/*.xml
}

# TODO: use SDL dummy driver for -set and -get (write a wrapper function)
SetDefaults()
{
	"$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution '640x480@60'
	StartX
	export DISPLAY=:0
	$Wiz -set Video_Ratio '4_3'
	$Wiz -set Video_Resolution '640x480'
	$Wiz -set Video_Refresh '60'
	$Wiz -set Video_Output 'VGA' #TODO: parse X log and extract autodetected output
	$Wiz -set AudioConnector 'Analog Stereo'
	GetAudioMixerSettings
	$Wiz -set AudioVolumeMin "$AudioVolumeMin"
	$Wiz -set AudioVolumeMax "$AudioVolumeMax"
	$Wiz -set AudioVolumeCurrent "$AudioVolumeCurrent"
	$Wiz -set AudioVolumeIncrement '1'
	$Wiz -set DolbyTest '0'
	$Wiz -set DTSTest '0'
}

Done=0
while [[ "$Done" -eq 0 ]]; do
	CleanUp
	SetDefaults
	if "$BaseDir"/AVWizardWrapper.sh; then
		Done=1
	fi
	StopX
done
