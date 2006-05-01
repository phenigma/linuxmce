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
	WizSet Video_Ratio '4_3'
	WizSet VideoResolution '640x480'
	WizSet VideoRefresh '60'
	WizSet Video_Output 'VGA' #TODO: parse X log and extract autodetected output
	WizSet AudioConnector 'Analog Stereo'
	GetAudioMixerSettings
	WizSet AudioVolumeMin "$AudioVolumeMin"
	WizSet AudioVolumeMax "$AudioVolumeMax"
	WizSet AudioVolumeCurrent "$AudioVolumeCurrent"
	WizSet AudioVolumeIncrement '1'
	WizSet DolbyTest '0'
	WizSet DTSTest '0'
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
