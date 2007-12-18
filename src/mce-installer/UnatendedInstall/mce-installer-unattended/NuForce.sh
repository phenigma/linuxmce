#!/bin/bash

set -x
exec &>/tmp/NuForce.log

if [[ -z "$NuForce" ]]; then
	exit
fi

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh

DEVICECATEGORY_Media_Director=8

DEVICETEMPLATE_Media_Plugin=2
DEVICETEMPLATE_OnScreen_Orbiter=62
DEVICETEMPLATE_Asterisk=45
DEVICETEMPLATE_MythTV_Player=35

DEVICEDATA_ScreenWidth=100
DEVICEDATA_ScreenHeight=101
DEVICEDATA_PK_Skin=24
DEVICEDATA_PK_Size=25
DEVICEDATA_Video_settings=89
DEVICEDATA_Connector=68
DEVICEDATA_TV_Standard=229
DEVICEDATA_Audio_Settings=88
DEVICEDATA_Type=47

ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
OrbiterDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_OnScreen_Orbiter")
MediaPluginDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_Media_Plugin")

Packages()
{
	RemovePkgs=(
		asterisk asterisk-config asterisk-mysql asterisk-pluto asterisk-sounds-main freepbx pluto-asterisk
		asterisk-perl libct3 libiksemel3 libpri1.2 libradiusclient-ng2 libsqlite0 libtonezone0 php-db php-pear
		pluto-proxy-orbiter pluto-voicemail-monitor zaptel-modules-2.6.22-14-generic
		mythtv-backend mythtv-common mythtv-database mythtv-frontend mythtv-transcode-utils mythweb
		pluto-mythtv-player pluto-mythtv-plugin libmyth-0.20
	)

	InstallPkgs=(
		id-my-disc egalax pluto-slim-server-streamer
	)

	for Pkg in "${RemovePkgs[@]}"; do
		apt-get -y -f --purge remove "$Pkg"
	done

	for Pkg in "${InstallPkgs[@]}"; do
		apt-get -y -f install "$Pkg"
	done
}

AVWizardReplacement()
{
	UI_Normal_Horizontal=1

	OrbiterWidth=800
	OrbiterHeight=600
	Video_settings="800 600/60"
	Video_Connector="VGA"
	PK_Skin=12 # NuForce
	PK_Size=1 # 800x600
	NewAudioSetting="S"

	Queries=(
		"UPDATE Device_DeviceData SET IK_DeviceData='$OrbiterWidth' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_ScreenWidth'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$OrbiterHeight' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_ScreenHeight'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$Video_settings' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$Video_Connector' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Connector'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$PK_Skin' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_PK_Skin'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$PK_Size' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_PK_Size'"
		"UPDATE Orbiter SET Regen=1 WHERE PK_Orbiter='$OrbiterDev'"
		"REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData) VALUES('$ComputerDev', '$DEVICEDATA_Audio_Settings', '$NewAudioSetting')"
	)

	for Q in "${Queries[@]}"; do
		RunSQL "$Q"
	done

	UI_SetOptions "$OrbiterDev" 0 0 "$UI_Normal_Horizontal"

	/usr/pluto/bin/Xconfigure.sh --force-vesa --defaults

AVWizardRun="#!/bin/bash
cp /etc/X11/xorg.conf{.nuforce,}
amixer sset IEC958 unmute
alsactl store
"

	echo -n "$AVWizardRun" >/usr/pluto/bin/AVWizard_Run.sh
	echo "#!/bin/bash" >/usr/pluto/bin/Xconfigure.sh
	echo "#!/bin/bash" >/usr/pluto/bin/X-CheckSettings.sh

	ConfSet AVWizardDone 0
	ConfSet UseVideoWizard 0
}

AddEgalaxToXorgConf()
{
	Section='
Section "InputDevice"
	Identifier "EETI"
	Driver "egalax"
	Option "Device" "events"
	Option "Parameters" "/etc/egalax.cal"
	Option "ScreenNo" "0"
EndSection
'

	echo "$Section" >>/etc/X11/xorg.conf

	ServerLayout=NoServerLayout
	while read line; do
		line="${line#+}"
		line="${line%+}"
		if [[ "$line" == *'Section "ServerLayout"'* ]]; then
			ServerLayout=ServerLayout
		fi
		if [[ "$ServerLayout" == ServerLayout && "$line" == *"EndSection"* ]]; then
			ServerLayout=NoServerLayout
			echo $'\tInputDevice "EETI" "SendCoreEvents"'
		fi
		echo "$line"
	done < <(cat /etc/X11/xorg.conf | sed -r 's,^|$,+,g') >/etc/X11/xorg.conf.nuforce
}

DatabaseDefaults()
{
	Queries=(
		"UPDATE Device_DeviceData SET IK_DeviceData='flac' WHERE FK_Device='$MediaPluginDev' AND FK_DeviceData='$DEVICEDATA_Type'"
		"INSERT INTO Room(FK_Installation, FK_RoomType, Description) VALUES('$PK_Installation', 1, 'NuForce')"
		"UPDATE Device SET FK_Room=1 WHERE PK_Device=1"
		"UPDATE Device_DeviceData SET IK_DeviceData='' WHERE FK_DeviceData=28"
		"UPDATE Device SET Disabled=1 WHERE FK_DeviceTemplate IN ($DEVICETEMPLATE_Asterisk, $DEVICETEMPLATE_MythTV_Player)"
		"INSERT INTO Users(UserName) VALUES('nuforce')"
	)

	for Q in "${Queries[@]}"; do
		RunSQL "$Q"
	done

	/usr/pluto/bin/Timezone_Detect.sh
	/usr/pluto/bin/SetPasswords.sh 1 nuforce

	Q="
		INSERT INTO QuickStartTemplate (Description, \`Binary\`, Arguments, Icon, WindowClass)
		VALUES ('TouchKit', '/usr/bin/TouchKit', '', '', 'TouchKit.TouchKit');
		SELECT LAST_INSERT_ID()
	"
	QST=$(RunSQL "$Q")
	SortOrder=$(RunSQL "SELECT MAX(SortOrder)+1 FROM Device_QuickStart")
	Q="
		INSERT INTO Device_QuickStart (FK_Device, Description, SortOrder,FK_QuickStartTemplate)
		VALUES ('$ComputerDev', 'TouchKit', '$SortOrder', '$QST');
	"
	RunSQL "$Q"
}

Packages
AVWizardReplacement
AddEgalaxToXorgConf
DatabaseDefaults
