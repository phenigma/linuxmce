#!/bin/bash

set -x
exec &>/tmp/NuForce.log

if [[ -z "$NuForce" ]]; then
	exit
fi

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh

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
	DEVICEDATA_ScreenWidth=100
	DEVICEDATA_ScreenHeight=101
	DEVICEDATA_PK_Skin=24
	DEVICEDATA_PK_Size=25
	DEVICEDATA_Video_settings=89
	DEVICEDATA_Connector=68
	DEVICEDATA_TV_Standard=229
	DEVICEDATA_Audio_Settings=88

	DEVICETEMPLATE_OnScreen_Orbiter=62
	DEVICECATEGORY_Media_Director=8

	UI_Normal_Horizontal=1

	ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
	OrbiterDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_OnScreen_Orbiter")

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

	echo "#!/bin/bash" >/usr/pluto/bin/AVWizard_Run.sh
	echo "#!/bin/bash" >/usr/pluto/bin/Xconfigure.sh

	ConfSet AVWizardDone 1
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
		if [[ "$line" == *'Section "ServerLayout"'* ]]; then
			ServerLayout=ServerLayout
		fi
		if [[ "$ServerLayout" == ServerLayout && "$line" == *"EndSection"* ]]; then
			ServerLayout=NoServerLayout
			echo $'\tInputDevice "EETI" "SendCoreEvents"'
		fi
		echo "$line"
	done </etc/X11/xorg.conf >/etc/X11/xorg.conf.$$
	mv /etc/X11/xorg.conf{.$$,}
}

DatabaseDefaults()
{
	DEVICETEMPLATE_Media_Plugin=2
	DEVICEDATA_Type=47

	MediaPluginDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_Media_Plugin")
	Queries=(
		"UPDATE Device_DeviceData SET IK_DeviceData='flac' WHERE FK_Device='$MediaPluginDev' AND FK_DeviceData='$DEVICEDATA_Type'"
		"INSERT INTO Room(FK_Installation, FK_RoomType, Description) VALUES('$PK_Installation', 1, 'NuForce')"
		"UPDATE Device SET FK_Room=1 WHERE PK_Device=1"
		"UPDATE Device_DeviceData SET IK_DeviceData='' WHERE FK_DeviceData=28"
	)

	for Q in "${Queries[@]}"; do
		RunSQL "$Q"
	done

	/usr/pluto/bin/Timezone_Detect.sh
}

Packages
AVWizardReplacement
AddEgalaxToXorgConf
DatabaseDefaults
