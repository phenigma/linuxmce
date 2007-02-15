#!/bin/bash

exit 0

if [[ ! -r /tmp/mce_wizard_data.sh ]] ;then
	echo "ERROR: Cannot find wizard data";
	exit 1
fi

function Setup_Network_Intefaces {
	if [[ $c_netExtUseDhcp  == "1" ]] ;then
		echo "iface $c_netExtName inet dhcp" > /etc/network/interfaces
	else
		echo > /etc/network/interfaces
		echo "auto $c_netExtName" >> /etc/network/interfaces
		echo "iface $c_netExtName inet static" >> /etc/network/interfaces
		echo "address $c_netExtIP" >> /etc/network/interfaces
		echo "netmask $c_netExtMask" >> /etc/network/interfaces
		echo "gateway $c_netExtGateway" >> /etc/network/interfaces

		echo > /etc/resolv.conf
		echo "nameserver $c_netExtDNS1" >> /etc/resolv.conf
	fi

	invoke-rc.d networking restart
}

function Setup_Apt_Conffiles {
	## Setup apt sources.list
	local Sources="# Pluto sources - start
deb http://archive.ubuntu.com/ubuntu edgy main restricted multiverse universe
deb http://10.0.0.82/ ./
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
UseVideoWizard = 1
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
}

function Install_DCERouter {
	apt-get update
	apt-get -y -f install pluto-dcerouter
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
	if [[ c_deviceType == 2 ]] ;then
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
	## Setup /etc/hosts
	selectedInterface=$(grep 'iface..*eth' /etc/network/interfaces | awk '{print $2}')
	dcerouterIP=$(ifconfig $selectedInterface | awk 'NR==2' | cut -d: -f2 | cut -d' ' -f1)

	hosts="
127.0.0.1       localhost.localdomain   localhost
$dcerouterIP    dcerouter $(/bin/hostname)

# The following lines are desirable for IPv6 capable hosts
::1     ip6-localhost ip6-loopback
fe00::0 ip6-localnet
ff00::0 ip6-mcastprefix
ff02::1 ip6-allnodes
ff02::2 ip6-allrouters
ff02::3 ip6-allhosts
"
	echo "$hosts" >/etc/hosts

	if [[ "$NetworkInput" != "" ]] ;then
		error=false
		Network=""
		Digits_Count=0
		for Digits in $(echo "$NetworkInput" | tr '.' ' ') ;do
			[[ "$Digits" == *[^0-9]* ]] && error=true

			[[ $Digits -lt 0 || $Digits -gt 255 ]] && error=true


			if [[ "$Network" == "" ]] ;then
				Network="$Digits"
			else
				Network="${Network}.${Digits}"
			fi

			Digits_Count=$(( $Digits_Count + 1 ))
		done
		[[ $Digits_Count -lt 1 || $Digits_Count -gt 3 ]] && error=true

		[[ "$error" == "true" ]] && continue
	else
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
	if [[ "$c_netIntName" != "" ]] ;then
		IntIf="$c_netExtName:0"
	else
		InfIf="$c_netIntName"
	fi

	if [[ "$c_netExtUseDhcp" == "0" ]] ;then
		NETsetting="$c_netExtName,$c_netExtIP,$c_netExtMask,$c_netExtGateway,$c_netExtDNS1|$InfIf,$InfIP,$IntNetmask"
	else
		NETsetting="$c_netExtName,dhcp|$InfIf,$InfIP,$IntNetmask"
	fi
	

	NETsetting=$(/usr/pluto/install/Initial_Network_Config.sh "$Network" "$Digits_Count")
	DHCPsetting=$(/usr/pluto/install/Initial_DHCP_Config.sh "$Network" "$Digits_Count")

	Q="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES('$Core_PK_Device',32,'$NETsetting')"
	RunSQL "$Q"
	if [[ "$DHCP" != n && "$DHCP" != N ]]; then
		Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
			VALUES($Core_PK_Device, 28, '$DHCPsetting')"
		RunSQL "$Q"
	fi

	/usr/pluto/bin/Network_Setup.sh
	/usr/pluto/bin/DHCP_config.sh

}

Core_PK_Device=""
Setup_Network_Intefaces
Setup_Apt_Conffiles
Setup_Pluto_Conf
Setup_NIS
Install_DCERouter
Create_And_Config_Devices
Configure_Network_Options

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/Timezone_Detect.sh

