#!/bin/bash

. /usr/pluto/bin/Utils.sh

rm -rf /tmp/mce_installer_error
InstallerLogFile="/var/log/mce-installer-$(date +%s).log"
echo "--- start mce wizard data ---" >> $InstallerLogFile
cat /tmp/mce_wizard_data.sh >> $InstallerLogFile
echo "--- end mce wizard data ---" >> $InstallerLogFile
exec &> >(tee $InstallerLogFile)

function ExitInstaller {
	echo 
	echo "ERROR : $*" >&2
	echo
	echo "$*" > /tmp/mce_installer_error
	exit 1
}


function StatsMessage {
	echo "MESSAGE: $*"
}

if [[ ! -r /tmp/mce_wizard_data.sh ]] ;then
	ExitInstaller "Wizard Information is corrupted or missing."
fi
. /tmp/mce_wizard_data.sh

function Setup_Network_Intefaces {
	StatsMessage "Seting Up Networking"
	# If user is was using dhcp and now wants static ip is safe to kill this here
	killall dhclient  &>/dev/null
	killall dhclient3 &>/dev/null

	if [[ $c_netExtUseDhcp  == "1" ]] ;then
		echo "iface $c_netExtName inet dhcp" > /etc/network/interfaces
		dhclient $c_netExtName || ExitInstaller "Failed to get an IP address for the external nic via DHCP."
	else
		echo > /etc/network/interfaces
		echo "auto lo" >> /etc/network/interfaces
		echo "iface lo inet loopback" >> /etc/network/interfaces

		echo "" >> /etc/network/interfaces
		echo "auto $c_netExtName" >> /etc/network/interfaces
		echo "iface $c_netExtName inet static" >> /etc/network/interfaces
		echo "address $c_netExtIP" >> /etc/network/interfaces
		echo "netmask $c_netExtMask" >> /etc/network/interfaces
		echo "gateway $c_netExtGateway" >> /etc/network/interfaces

		echo > /etc/resolv.conf
		echo "nameserver $c_netExtDNS1" >> /etc/resolv.conf


		ifconfig $c_netIntName down
		ifconfig $c_netExtName down
		ifconfig $c_netExtName $c_netExtIP netmask $c_netExtMask up || ExitInstaller "Cannot set an IP address to '$c_netExtName'."

		route del default gw 2>/dev/null
		route add default gw $c_netExtGateway 2>/dev/null || ExitInstaller "Cannot setup default gateway to '$c_netExtGateway'."
	fi

}

function Replace_Mirror {
	local mirror=$1
	local flag='0'
	local newlines="deb $mirror edgy main restricted universe multiverse
deb $mirror edgy-security main restricted universe multiverse
deb $mirror edgy-updates  main restricted universe multiverse"

	while read line ;do
		if [[ "$line" == '# Choosed mirror - end' ]] ;then
			echo "$newlines" >> /etc/apt/sources.list.$$
			flag='0'
		fi
		if [[ "$line" == '# Choosed mirror - start' ]] ;then
			echo "$line" >> /etc/apt/sources.list.$$		
			flag='1'
		fi
		if [[ "$flag" == '0' ]] ;then
			echo "$line" >> /etc/apt/sources.list.$$
		fi
	done < /etc/apt/sources.list

	mv -f /etc/apt/sources.list.$$ /etc/apt/sources.list
}

function Setup_Apt_Conffiles {
	StatsMessage "Setting up APT Package Manager"
	
if ! BlacklistConfFiles '/etc/apt/sources.list' ;then
	## Backup initial apt sources.list
	if [ ! -e /etc/apt/sources.list.pbackup ] ;then
		cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
	fi
	## Setup apt sources.list
	local Sources="# Pluto sources - start
deb file:/usr/pluto/deb-cache/ ./
# Choosed mirror - start
deb $c_installMirror edgy main restricted multiverse universe
deb $c_installMirror edgy-security main restricted multiverse universe
deb $c_installMirror edgy-updates main restricted multiverse universe
# Choosed mirror - end
deb http://linuxmce.com/ubuntu ./
#deb http://10.0.0.82/ ./
# Pluto sources - end"
	if ! grep -qF '# Pluto sources - start' /etc/apt/sources.list ;then
		echo "$Sources" >>/etc/apt/sources.list
	else
		Replace_Mirror "$c_installMirror"
	fi
fi

	local SourcesOffline="# Pluto sources offline - start
deb file:/usr/pluto/deb-cache/ ./
# Pluto sources offline - end"
	echo "$SourcesOffline" >/etc/apt/sources.list.offline

## Setup pluto's apt.conf
	pluto_apt_conf='// Pluto apt conf add-on
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
Acquire::http::timeout "10";
Acquire::ftp::timeout "10";
APT::Get::AllowUnauthenticated "true";
'
	echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto

	ln -s /usr/pluto/var/apt.conf.offline /etc/apt/apt.conf.d/99offline

	StatsMessage "Preseeding debconf options"
	./mce-installer-preseed.sh
	apt-get update
}


function Setup_Pluto_Conf {
	StatsMessage "Generating Default Config File"
	PlutoConf="# Pluto config file
MySqlHost = localhost
MySqlUser = root
MySqlPassword =
MySqlDBName = pluto_main
DCERouter = localhost
MySqlPort = 3306
DCERouterPort = 3450
PK_Device = 1
Activation_Code = 1111
PK_Installation = 1
PK_Users = 1
PK_Distro = 14
Display = 4
OfflineMode = false
LogLevels = 1,5,7,8
"
	echo "$PlutoConf" > /etc/pluto.conf
	chmod 777 /etc/pluto.conf &>/dev/null
}

function Setup_NIS {
	StatsMessage "Configuring Network Information Service (NIS)"
	## Put a temporar nis config files that will prevent ypbind to start
	echo "
NISSERVER=false
NISCLIENT=false
YPPWDDIR=/etc
YPCHANGEOK=chsh
NISMASTER=
YPSERVARGS=
YPBINDARGS=
YPPASSWDDARGS=
YPXFRDARGS=
" > /etc/default/nis

echo > /etc/init.d/yp.conf
}

function Install_DCERouter {
	StatsMessage "Installing LinuxMCE Base Software"
	apt-get update
	apt-get -y -f install pluto-dcerouter || ExitInstaller "Failed to install and configure the base software"
}

function UI_SetOptions {
	local OrbiterDev="$1"
	local OpenGLeffects="$2"
	local AlphaBlending="$3"
	local UI_Version="$4"

	DEVICEDATA_Use_OpenGL_effects=172
	DEVICEDATA_Use_alpha_blended_UI=169
	DEVICEDATA_PK_UI=104

	# disable OpenGL effects
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	     VALUES ('$OrbiterDev', '$DEVICEDATA_Use_OpenGL_effects', '$OpenGLeffects')"
	RunSQL "$Q"

	# disable alpha blending
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	     VALUES ('$OrbiterDev', '$DEVICEDATA_Use_alpha_blended_UI', '$AlphaBlending')"
	RunSQL "$Q"

	# set UI	
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	     VALUES ('$OrbiterDev', '$DEVICEDATA_PK_UI', '$UI_Version')"
	RunSQL "$Q"

}

function Create_And_Config_Devices {

	. /usr/pluto/bin/SQL_Ops.sh

	DEVICE_TEMPLATE_Core=7
	DEVICE_TEMPLATE_MediaDirector=28

	## Update some info in the database
	Q="INSERT INTO Installation(Description, ActivationCode) VALUES('Pluto', '1111')"
	RunSQL "$Q"

	## Create the Core device and set it's description
	StatsMessage "Setting up your computer to act as a 'Core'"
	Core_PK_Device=$(/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_Core | tee /dev/stderr | tail -1)
	Q="UPDATE Device SET Description='CORE' WHERE PK_Device='$Core_PK_Device'"
	RunSQL "$Q"

	## Create a hybrid if needed
	if [[ "$c_deviceType" == "2" ]] ;then
		StatsMessage "Setting up you computer to act as a 'Media Director'"
		Hybrid_DT=$(/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_MediaDirector -C "$Core_PK_Device")
		Q="UPDATE Device SET Description='The core/hybrid' WHERE PK_Device='$Hybrid_DT'"
		RunSQL "$Q"
 
		## Set UI interface
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Hybrid_DT'"
		OrbiterDevice=$(RunSQL "$Q")

		case "$c_installUI" in
			"0")
				# select UI1
				UI_SetOptions "$OrbiterDevice" 0 0 1
			;;
			"1")
				# select UI2 without alpha blending
				UI_SetOptions "$OrbiterDevice" 1 0 4
			;;
			"2")
				# select UI2 with alpha blending
				UI_SetOptions "$OrbiterDevice" 1 1 4
			;;
			*)
				echo "Unknown UI version: $c_installUI"
			;;
		esac
	fi

	StatsMessage "Configuring the LinuxMCE devices"
        # "DCERouter postinstall"
        devices=$(echo "SELECT PK_Device FROM Device;" | /usr/bin/mysql pluto_main | tail +2 | tr '\n' ' ')

	for i in $devices; do
		echo "Running device $i"

		#run the following with the device
		# Add missing data by template
		Q1="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT PK_Device,DeviceTemplate_DeviceData.FK_DeviceData,DeviceTemplate_DeviceData.IK_DeviceData
		FROM Device
		JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceTemplate_DeviceData on DeviceTemplate_DeviceData.FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceTemplate_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=$i;"

		# Add for the category
		Q2="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT PK_Device,DeviceCategory_DeviceData.FK_DeviceData,DeviceCategory_DeviceData.IK_DeviceData
		FROM Device
		JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceTemplate.FK_DeviceCategory
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=$i;"

		# Add for parent's category
		Q3="INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData)
		SELECT PK_Device,DeviceCategory_DeviceData.FK_DeviceData,DeviceCategory_DeviceData.IK_DeviceData
		FROM Device
		JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceCategory ON DeviceTemplate.FK_DeviceCategory=PK_DeviceCategory
		JOIN DeviceCategory_DeviceData on DeviceCategory_DeviceData.FK_DeviceCategory=DeviceCategory.FK_DeviceCategory_Parent
		LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND Device_DeviceData.FK_DeviceData=DeviceCategory_DeviceData.FK_DeviceData
		WHERE Device_DeviceData.FK_Device IS NULL AND PK_Device=$i;"

		(echo "$Q1"; echo "$Q2"; echo "$Q3";) | /usr/bin/mysql pluto_main
	done

	/usr/pluto/bin/Update_StartupScrips.sh
}

function Configure_Network_Options {
	StatsMessage "Configuring your internal network"
	. /usr/pluto/bin/SQL_Ops.sh

	## Setup /etc/hosts
	echo > /etc/hosts
	echo "127.0.0.1 localhost.localdomain localhost" >> /etc/hosts
	echo "$c_netExtIP dcerouter $(/bin/hostname)"    >> /etc/hosts

	error=false
	Network=""
	Digits_Count=0

	for Digits in $(echo "$c_netIntIPN" | tr '.' ' ') ;do
		[[ "$Digits" == *[^0-9]* ]]            && error=true
		[[ $Digits -lt 0 || $Digits -gt 255 ]] && error=true

		if [[ "$Network" == "" ]] ;then
			Network="$Digits"
		else
			Network="${Network}.${Digits}"
		fi

		Digits_Count=$(( $Digits_Count + 1 ))
	done
	[[ $Digits_Count -lt 1 || $Digits_Count -gt 3 ]] && error=true

	if [[ "$error" == "true" ]] ;then
		Network="192.168.80"
		Digits_Count="3"
	fi

	IntIP="$Network"
	IntNetmask=""
	for i in `seq 1 $Digits_Count` ;do
		if [[ "$IntNetmask" == "" ]] ;then
			IntNetmask="255"
		else
			IntNetmask="${IntNetmask}.255"
		fi
	done
	for i in `seq $Digits_Count 3` ;do
		if [[ $i == "3" ]] ;then
			IntIP="${IntIP}.1"
		else
			IntIP="${IntIP}.0"
		fi

		IntNetmask="${IntNetmask}.0"
	done

	if [[ "$c_netIntName" == "" ]] ;then
		IntIf="$c_netExtName:0"
	else
		IntIf="$c_netIntName"
	fi

	if [[ "$c_netExtUseDhcp" == "0" ]] ;then
		NETsetting="$c_netExtName,$c_netExtIP,$c_netExtMask,$c_netExtGateway,$c_netExtDNS1|$IntIf,$IntIP,$IntNetmask"
	else
		NETsetting="$c_netExtName,dhcp|$IntIf,$IntIP,$IntNetmask"
	fi
	
	DHCPsetting=$(/usr/pluto/install/Initial_DHCP_Config.sh "$Network" "$Digits_Count")

	Q="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES('$Core_PK_Device',32,'$NETsetting')"
	RunSQL "$Q"
	if [[ "$DHCP" != n && "$DHCP" != N ]]; then
		Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
			VALUES($Core_PK_Device, 28, '$DHCPsetting')"
		RunSQL "$Q"
	fi

}

function CreateDebCache() {
	StatsMessage "Caching LinuxMCE CD on the harddrive"
	mkdir -p /usr/pluto/deb-cache
	cp /media/cdrom/*.deb /usr/pluto/deb-cache/ ||  ExitInstaller "Failed to read correctly from the LinuxMCE CDROM"
	cp /media/cdrom/Packages* /usr/pluto/deb-cache/
}

function Setup_DebCache() {
	local OK=0

#	apt-get -y install dpkg-dev
	while [[ "$OK" == 0 ]]; do
		case "$c_installType" in
			1) # ISO on CD
				echo "Mounting CD"
				mount /dev/cdrom /media/cdrom 2>/dev/null
				CreateDebCache
				umount /media/cdrom 2>/dev/null
				OK=1
			;;
			2) # ISO in /var/linuxmce/linux-mce.iso
				echo "Installing from ISO"
				mount -o loop /var/linuxmce/linux-mce.iso /media/cdrom
				CreateDebCache
				umount /media/cdrom
				OK=1
			;;
			3) # Net install; Nothing to do
				OK=1
				:
			;;
			*) echo "Bad install type: '$c_InstallType'" 
				OK=1
			;;
		esac
	done
}

Core_PK_Device="0"
if [[ "$c_netExtKeep" == "true" ]] ;then
	Setup_Network_Intefaces
fi
Setup_Apt_Conffiles
Setup_Pluto_Conf
Setup_NIS
Setup_DebCache
Install_DCERouter
Create_And_Config_Devices
Configure_Network_Options

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/Timezone_Detect.sh
/usr/pluto/bin/Network_Setup.sh
/usr/pluto/bin/DHCP_config.sh

StatsMessage "Building a disk image for your Diskless Media Directors"
/usr/pluto/bin/Diskless_CreateTBZ.sh


mkdir -p /usr/pluto/deb-cache
sed -i 's/"nv"/"vesa"/g' /etc/X11/xorg.conf
sed -i 's/"nvidia"/"vesa"/g' /etc/X11/xorg.conf

if [[ "$c_startupType" == "1" ]] ;then
	mv /etc/init.d/gdm /etc/init.d/gdm.saved
	echo "
start on runlevel-2

stop on shutdown
stop on runlevel-3
stop on runlevel-4
stop on runlevel-5

script
/usr/pluto/bin/Startup_Core-Hybrid.sh
end script
" > /etc/event.d/pluto
elif [[ "$c_startupType" == "2" ]] ;then
	echo "
start on runlevel-2

stop on shutdown
stop on runlevel-3
stop on runlevel-4
stop on runlevel-5

script
/usr/share/mce-launcher/scripts/mce-launcher-core.sh
end script
" > /etc/event.d/pluto
fi

apt-get -y dist-upgrade || ExitInstaller "Failed while upgrading the system. Installation finished but you system might be left in a unstable state."

StatsMessage "Installation Finished"
