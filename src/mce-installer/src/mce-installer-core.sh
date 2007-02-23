#!/bin/bash

if [[ ! -r /tmp/mce_wizard_data.sh ]] ;then
	echo "ERROR: Cannot find wizard data";
	exit 1
fi

. /tmp/mce_wizard_data.sh

function Setup_Network_Intefaces {
	if [[ $c_netExtUseDhcp  == "1" ]] ;then
		echo "iface $c_netExtName inet dhcp" > /etc/network/interfaces
		dhclient $c_netExtName
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

		ifconfig $c_netIntName $c_netExtIP netmask $c_netExtMask up
		route add default gw $c_netExtGateway
	fi

}

function Setup_Apt_Conffiles {
	## Setup apt sources.list
	local Sources="# Pluto sources - start
deb http://archive.ubuntu.com/ubuntu edgy main restricted multiverse universe
deb http://ubuntu.plutohome.com/ ./
deb file:/usr/pluto/deb-cache/ sarge main
# Pluto sources - end"
	echo "$Sources" >/etc/apt/sources.list

	local SourcesOffline="# Pluto sources offline - start
deb file:/usr/pluto/deb-cache/ sarge main
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

	./mce-installer-preseed.sh
}


function Setup_Pluto_Conf {
	PlutoConf="# Pluto config file
MySqlHost = localhost
MySqlUser = root
MySqlPassword =
MySqlDBName = pluto_main
DCERouter = localhost
MySqlPort = 3306
DCERouterPort = 3450
#
PK_Device = 1
Activation_Code = 1111
PK_Installation = 1
PK_Users = 1
PK_Distro = 14
Display = 4
OfflineMode = false
"
	echo "$PlutoConf" > /etc/pluto.conf
	chmod 777 /etc/pluto.conf &>/dev/null
}

function Setup_NIS {
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
	apt-get update
	apt-get -y -f install pluto-dcerouter
	local err=$?
	if [[ "$err" != "0" ]] ;then
		echo "-------------------------------------------------------"
		echo "-                                                     -"
		echo "-           !!  Installation Failed !!                -"
		echo "-                                                     -"
		echo "-------------------------------------------------------"

		exit 1
	fi
}

function Create_And_Config_Devices {

	. /usr/pluto/bin/SQL_Ops.sh

	DEVICE_TEMPLATE_Core=7
	DEVICE_TEMPLATE_MediaDirector=28

	## Update some info in the database
	Q="INSERT INTO Installation(Description, ActivationCode) VALUES('Pluto', '1111')"
	RunSQL "$Q"

	## Create the Core device and set it's description
	Core_PK_Device=$(/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_Core | tee /dev/stderr | tail -1)
	Q="UPDATE Device SET Description='CORE' WHERE PK_Device='$Core_PK_Device'"
	RunSQL "$Q"

	## Create a hybrid if needed
	if [[ "$c_deviceType" == "2" ]] ;then
		Hybrid_DT=$(/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_MediaDirector -C "$Core_PK_Device")
		Q="UPDATE Device SET Description='The core/hybrid' WHERE PK_Device='$Hybrid_DT'"
		RunSQL "$Q"
	fi

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
	mkdir -p /usr/pluto/deb-cache
	cp /media/cdrom/*.deb /usr/pluto/deb-cache/
	pushd /usr/pluto/deb-cache >/dev/null
	dpkg-scanpackages . /dev/null | gzip -9c > Packages.gz
	popd >/dev/null
}

function Setup_DebCache() {
	local OK=0

	apt-get -y install dpkg-dev
	while [[ "$OK" == 0 ]]; do
		case "$c_installType" in
			1) # ISO on CD
				echo "Mounting CD"
				mount /dev/cdrom /media/cdrom
				CreateDebCache
				umount /media/cdrom
			;;
			2) # ISO in /var/linuxmce/linux-mce.iso
				echo "Installing from ISO"
				mount -o loop /var/linuxmce/linux-mce.iso /media/cdrom
				CreateDebCache
				umount /media/cdrom
			;;
			3) # Net install; Nothing to do
				:
			;;
			*) echo "Bad install type: '$c_InstallType'" ;;
		esac
	done
}

Core_PK_Device="0"
Setup_Network_Intefaces
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


mkdir -p /usr/pluto/deb-cache
sed -i 's/"nv"/"nvidia"/g' /etc/X11/xorg.conf

echo 
echo
echo
echo
echo "-------------------------------------------------------"
echo "-              Installation Finished                  -"
echo "-                                                     -"
echo "-                                                     -"
echo "- Reboot you computer and then run the Start_Pluto.sh -"
echo "- script to get pluto hybrid/core started.            -"
echo "-                                                     -"
echo "-------------------------------------------------------"
