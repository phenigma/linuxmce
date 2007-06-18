#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICECATEGORY_Media_Director=8

DEVICEDATA_ScreenWidth=100
DEVICEDATA_ScreenHeight=101
DEVICEDATA_Video_settings=89
DEVICEDATA_Audio_settings=88
DEVICEDATA_PK_Size=25
DEVICEDATA_Connector=68
DEVICEDATA_TV_Standard=229

ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
OrbiterDev=$(FindDevice_Template "$ComputerDev" "$DEVICETEMPLATE_OnScreen_Orbiter")

ParseParms()
{
	local i
	for ((i = 1; i <= "$#"; i++)); do
		case "${!i}" in
			--resolution)
				((i++))
				Resolution="${!i}"
			;;
			--video-connector)
				((i++))
				Output="${!i}"
			;;
			--tv-standard)
				((i++))
				TVStandard="${!i}"
			;;
			--audio-setting)
				((i++))
				AudioSetting="${!i}"
				AudioSetting_Set=y
			;;
		esac
	done
}

UpdateResolution()
{
	local Q
	local Width Height WidthHeight
	local Video_settings PK_Size

	if [[ -z "$Resolution" ]]; then
		return
	fi
	
	Refresh="${Resolution#*@}"
	if [[ -z "$Refresh" ]]; then
		Refresh=60
	fi
	WidthHeight="${Resolution%@*}"
	if [[ "$WidthHeight" != *x* ]]; then
		return
	fi
	
	Width="${WidthHeight%x*}"
	Height="${WidthHeight#*x}"
	Video_settings="$Width $Height/$Refresh"

	Q="SELECT PK_Size FROM Size WHERE Description LIKE '%$OrbiterResolutionName%'"
	PK_Size=$(RunSQL "$Q")

	SetDeviceData "$OrbiterDev" "$DEVICEDATA_ScreenWidth" "$Width"
	SetDeviceData "$OrbiterDev" "$DEVICEDATA_ScreenHeight" "$Height"
	SetDeviceData "$ComputerDev" "$DEVICEDATA_Video_settings" "$Video_settings"

	if [[ -n "$PK_Size" ]]; then
		SetDeviceData "$OrbiterDev" "$DEVICEDATA_PK_Size" "$PK_Size"
	fi
}

UpdateConnector()
{
	if [[ -z "$Output" ]]; then
		return
	fi

	if [[ " VGA DVI Component " == *" $Output "* ]]; then
		SetDeviceData "$ComputerDev" "$DEVICEDATA_Connector" "$Output"
	fi
}

UpdateTVStandard()
{
	if [[ -z "$TVStandard" ]]; then
		return
	fi

	SetDeviceData "$ComputerDev" "$DEVICEDATA_TV_Standard" "$TVStandard"
}

UpdateAudioSetting()
{
	if [[ "$AudioSetting_Set" != y ]]; then
		return
	fi

	SetDeviceData "$ComputerDev" "$DEVICEDATA_Audio_settings" "$AudioSetting"
}

ParseParms "$@"
UpdateResolution
UpdateConnector
UpdateTVStandard
UpdateAudioSetting
