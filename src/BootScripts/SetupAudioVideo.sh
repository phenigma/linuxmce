#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh

DEVICECATEGORY_Media_Director=8
DEVICECATEGORY_Video_Cards=125
DEVICECATEGORY_Sound_Cards=124

DEVICETEMPLATE_OnScreen_Orbiter=62

DEVICEDATA_Video_settings=89
DEVICEDATA_Audio_settings=88
DEVICEDATA_Reboot=236
DEVICEDATA_Connector=68
DEVICEDATA_TV_Standard=229
DEVICEDATA_Setup_Script=189

SettingsFile=/etc/pluto/lastaudiovideo.conf

# don't let KDE override xorg.conf
rm -f {/home/*,/root}/.kde/share/config/displayconfigrc

Reboot=NoReboot
ReloadX=NoReloadX

ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
OrbiterDev=$(FindDevice_Template "$ComputerDev" "$DEVICETEMPLATE_OnScreen_Orbiter")
VideoCardDev=$(FindDevice_Category "$ComputerDev" "$DEVICECATEGORY_Video_Cards")
SoundCardDev=$(FindDevice_Category "$ComputerDev" "$DEVICECATEGORY_Sound_Cards")

ReadConf()
{
	## config file format (direcly parsable by bash):
	# name=value
	if [[ -f "$SettingsFile" ]]; then
		while read line; do
			eval "OldSetting_$line"
		done <"$SettingsFile"
	fi
}

GetVideoSetting()
{
	local Q
	local VideoSetting
	
	VideoSetting=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_Video_settings")

	if [[ -n "$VideoSetting" ]]; then
		Refresh=$(echo $VideoSetting | cut -d '/' -f2)
		ResolutionInfo=$(echo $VideoSetting | cut -d '/' -f1)
		ResX=$(echo $ResolutionInfo | cut -d' ' -f1)
		ResY=$(echo $ResolutionInfo | cut -d' ' -f2)
		if [[ -z "$Refresh" || -z "$ResX" || -z "$ResY" ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "Xconfigure" "Malformed DeviceData: VideoSetting='$VideoSetting'"
			MalformedVideoSetting='Malformed VideoSetting'
		fi
	fi

	if [[ -z "$MalformedVideoSetting" ]]; then
		echo "$VideoSetting"
	fi
}

wmtweaks_default()
{
	echo '<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE mcs-option SYSTEM "mcs-option.dtd">

<mcs-option>
	<option name="Xfwm/UseCompositing" type="int" value="1"/>
</mcs-option>'
}

SetWMCompositor()
{
	local UseCompositing="$1"
	local WMTweaksFile
	local user

	for user in /home/*; do
		if [[ ! -d "$user" ]]; then
			continue
		fi
		WMTweaksFile="$user/.config/xfce4/mcs_settings/wmtweaks.xml"
		mkdir -p "$(dirname "$WMTweaksFile")"
		if [[ -f "$WMTweaksFile" ]]; then
			sed -i '/Xfwm\/UseCompositing/ s/value="."/value="'"$UseCompositing"'"/g' "$WMTweaksFile"
		else
			wmtweaks_default >"$WMTweaksFile"
		fi
	done
}

SaveSettings()
{
	local Var VarName

	for Var in ${!OldSetting_*}; do
		VarName="${Var#OldSetting_}"
		eval "Save_$VarName=\"${!Var}\""
	done

	for Var in ${!NewSetting_*}; do
		VarName="${Var#NewSetting_}"
		eval "Save_$VarName=\"${!Var}\""
	done

	for Var in ${!Save_*}; do
		VarName="${Var#Save_}"
		echo "$VarName=\"${!Var}\""
	done >"$SettingsFile"
}

VideoSettings_Check()
{
	local Update_XorgConf="NoXorgConf" Update_WMcompositor="NoWMcompositor"
	local DB_VideoSetting DB_OpenGL DB_AlphaBlending DB_Connector DB_TVStandard DB_Reboot

	DB_VideoSetting=$(GetVideoSetting)
	DB_OpenGL=$(OpenGLeffects)
	DB_AlphaBlending=$(AlphaBlendingEnabled)
	DB_Connector=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_Connector")
	DB_TVStandard=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_TV_Standard")
	DB_Reboot=$(GetDeviceData "$VideoCardDev" "$DEVICEDATA_Reboot")

	if [[ -n "$DB_VideoSetting" && "$DB_VideoSetting" != "$OldSetting_VideoSetting" ]]; then
		Update_XorgConf="XorgConf"
		NewSetting_VideoSetting="$DB_VideoSetting"
	fi
	if [[ -n "$DB_OpenGL" && "$DB_OpenGL" != "$OldSetting_OpenGL" ]]; then
		NewSetting_OpenGL="$DB_OpenGL"
	fi
	if [[ -n "$DB_AlphaBlending" && "$DB_AlphaBlending" != "$OldSetting_AlphaBlending" ]]; then
		Update_XorgConf="XorgConf"
		Update_WMcompositor="WMcompositor"
		NewSetting_AlphaBlending="$DB_AlphaBlending"
	fi
	if [[ -n "$DB_Connector" && "$DB_Connector" != "$OldSetting_Connector" ]]; then
		Update_XorgConf="XorgConf"
		NewSetting_Connector="$DB_Connector"
	fi
	if [[ -n "$DB_TVStandard" && "$DB_TVStandard" != "$OldSetting_TVStandard" ]]; then
		Update_XorgConf="XorgConf"
		NewSetting_TVStandard="$DB_TVStandard"
	fi

	if [[ "$Update_XorgConf" == "XorgConf" ]]; then
		/usr/pluto/bin/Xconfigure.sh
	fi
	if [[ "$Update_WMcompositor" == "WMcompositor" ]]; then
		SetWMCompositor "$DB_AlphaBlending"
	fi

	if [[ "$Update_XorgConf" == "XorgConf" || "$Update_WMcompositor" == "WMcompositor" ]]; then
		if [[ "$DB_Reboot" == 1 ]]; then
			Reboot="Reboot"
		else
			ReloadX="ReloadX"
		fi
	fi
}

AudioSettings_Check()
{
	local DB_AudioSetting DB_AudioScript DB_Reboot
	local ScriptPath

	DB_AudioSetting=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_Audio_settings")
	DB_Reboot=$(GetDeviceData "$AudioCardDev" "$DEVICEDATA_Reboot")

	if [[ "$DB_AudioSetting" != "$OldSetting_AudioSetting" ]]; then
		DB_AudioScript=$(GetDeviceData "$SoundCardDev" "$DEVICEDATA_Setup_Script")
		ScriptPath="/usr/pluto/bin/$DB_AudioScript"
		if [[ -n "$DB_AudioSetting" && -f "$ScriptPath" ]]; then
			"$ScriptPath" "$DB_AudioSetting"
		fi
		NewSetting_AudioSetting="$DB_AudioSetting"

		if [[ "$DB_Reboot" == 1 ]]; then
			Reboot="Reboot"
		fi
	fi
}

ReadConf
VideoSettings_Check
AudioSettings_Check
SaveSettings

if [[ "$Reboot" == Reboot ]]; then
	reboot
elif [[ "$ReloadX" == ReloadX ]]; then
	if [[ -f /etc/event.d/pluto || "$PK_Distro" == 1 ]]; then
		kill $(pidof X)
		if [[ "$PK_Distro" == 1 ]]; then
			sleep 1
			/usr/pluto/bin/Start_X.sh
		fi
	else
		/etc/init.d/kdm restart
	fi
fi
