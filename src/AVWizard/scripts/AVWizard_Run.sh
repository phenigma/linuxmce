#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh
. /usr/pluto/bin/Config_Ops.sh

CleanUp()
{
	rm -f /tmp/*.xml
}

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
	cp /etc/pluto/xine.conf "$XineConf"
	WizSet SoundTestFile '/usr/pluto/sound/avwizard_volume_test.mp3'
	WizSet DTSTestFile '/usr/pluto/sound/avwizard_dts_test.wav'
	WizSet DolbyTestFile '/usr/pluto/sound/avwizard_ac3_test.ac3'
	WizSet DefaultFontName '/usr/share/fonts/truetype/msttcorefonts/Arial.ttf'
}

# Update audio settings of this machine
UpdateAudioSettings()
{
	DEVICEDATA_Audio_Settings=88
	DEVICECATEGORY_Media_Director=8

	Q="
		SELECT FK_DeviceCategory
		FROM Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE PK_Device='$PK_Device'
	"
	Category=$(RunSQL "$Q")

	if [[ -z "$Category" ]]; then
		return 1
	elif [[ "$Category" != "$DEVICECATEGORY_Media_Director" ]]; then
		# it's the Core; search for its embedded MD
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			WHERE FK_Device_ControlledVia='$PK_Device' AND PK_DeviceCategory='$DEVICECATEGORY_Media_Director'
		"
		MD=$(RunSQL "$Q")
		if [[ -z "$MD" ]]; then
			# No embedded MD found
			return 1
		fi
	else
		MD="$PK_Device"
	fi

	Q="
		SELECT IK_DeviceData
		FROM Device_DeviceData
		WHERE FK_Device='$MD' AND FK_DeviceData='$DEVICEDATA_Audio_Settings'
	"
	AudioSetting=$(RunSQL "$Q")

	WizAudioConnector=$(WizGet 'AudioConntector')
	case "$WizAudioConnector" in
		'Analog Stereo')
			NewAudioSetting=
		;;
		'SPDIF Coaxial'|'SPDIF Optical')
			NewAudioSetting='K3'
		;;
	esac
	
	if [[ -z "$AudioSetting" ]]; then
		Q="
			INSERT INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
			VALUES('$MD', '$DEVICEDATA_Audio_Settings', '$NewAudioSetting')
		"
	else
		Q="
			UPDATE Device_DeviceData
			SET IK_DeviceData='$NewAudioSetting'
			WHERE FK_Device='$MD' AND FK_DeviceData='$DEVICEDATA_Audio_Settings'
		"
	fi
	RunSQL "$Q"
}

UpdateOrbiterDimensions()
{
	DEVICETEMPLATE_OnScreen_Orbiter=62
	DEVICETEMPLATE_OrbiterPlugin=12
	DEVICECATEGORY_Media_Director=8
	DEVICEDATA_ScreenWidth=100
	DEVICEDATA_ScreenHeight=101
	DEVICEDATA_PK_Size=25
	DEVICEDATA_Video_settings=89

	ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
	OrbiterDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_OnScreen_Orbiter")
	OrbiterResolutionName=$(WizGet 'VideoResolution')
	OrbiterAspectRatio=$(WizGet 'Video_Ratio')
	OrbiterRefresh=$(WizGet 'VideoRefresh')
	OrbiterResolutionFullName=$(Resolution_GetFullName "$OrbiterAspectRatio" "$OrbiterResolutionName")
	if [[ "$OrbiterResolutionFullName" == *=* ]]; then
		OrbiterResolution=${OrbiterResolutionFullName#*=}
		OrbiterResolution=${OrbiterResolution%i}
		OrbiterResolution=${OrbiterResolution%p}
	else
		OrbiterResolution="$OrbiterResolutionName"
	fi
	OrbiterWidth=${OrbiterResolution%x*}
	OrbiterHeight=${OrbiterResolution#*x}

	Q="UPDATE Device_DeviceData SET IK_DeviceData='$OrbiterWidth' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_ScreenWidth'"
	RunSQL "$Q"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$OrbiterHeight' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_ScreenHeight'"
	RunSQL "$Q"

	Video_settings="$OrbiterWidth $OrbiterHeight/$OrbiterRefresh"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$Video_settings' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings'"
	RunSQL "$Q"
	
	Q="SELECT PK_Size FROM Size WHERE Description LIKE '%$OrbiterResolutionName%'"
	PK_Size=$(RunSQL "$Q")
	if [[ -n "$PK_Size" ]]; then
		Q="UPDATE Device_DeviceData SET IK_DeviceData='$PK_Size' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_PK_Size'"
		RunSQL "$Q"
	fi

	# Regen Orbiter
	Q="UPDATE Orbiter SET Modification_LastGen=0 WHERE PK_Orbiter='$OrbiterDev'"
	RunSQL "$Q"
	Q="UPDATE Device SET NeedConfigure=1 WHERE PK_Device='$OrbiterDev'"
	RunSQL "$Q"
	/usr/pluto/bin/MessageSend "$DCERouter" -targetType template "$OrbiterDev" "$DEVICETEMPLATE_OrbiterPlugin" 1 266 2 "$OrbiterDev" 21 "-r" &>/dev/null
}

Done=0
while [[ "$Done" -eq 0 ]]; do
	CleanUp
	SetDefaults
	if "$BaseDir"/AVWizardWrapper.sh; then
		Done=1
	fi
done

# Finalize wizard: save settings
ConfSet "AVWizardDone" "1"
mv "$XF86Config" /etc/X11/xorg.conf
mv "$XineConf" /etc/pluto/xine.conf
alsactl store
UpdateAudioSettings
UpdateOrbiterDimensions
/usr/pluto/bin/MessageSend "$DCERouter" 0 -1000 7 1 # reload the router
