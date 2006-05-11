#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh
. /usr/pluto/bin/Config_Ops.sh

CleanUp()
{
	rm -f /tmp/*.xml
}

# TODO: use SDL dummy driver for -set and -get (write a wrapper function)
SetDefaults()
{
	"$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --defaults --resolution '640x480@60'
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
	WizSet XineConfigFile "$XineConf"
	WizSet SoundTestFile '/usr/pluto/sound/avwizard_volume_test.mp3'
	WizSet DTSTestFile '/usr/pluto/sound/avwizard_dts_test.wav'
	WizSet DolbyTestFile '/usr/pluto/sound/avwizard_ac3_test.ac3'
}

Done=0
while [[ "$Done" -eq 0 ]]; do
	CleanUp
	SetDefaults
	if "$BaseDir"/AVWizardWrapper.sh; then
		Done=1
	fi
done
ConfSet "AVWizardDone" "1"
mv "$XF86Config" /etc/X11/xorg.conf
mv "$XineConf" /etc/pluto/xine.conf
alsactl store
