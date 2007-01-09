#!/bin/bash
question_type="Hybrid"
question_use_internet_connection=0
question_netaddr="192.168.80"
question_run_dhcp=0


function Ask_Questions_Zenity {
zenity --info --text \
"I need to ask some basic questions
Please see http://plutohome.com/support click Quick Start Guide, 
and Installing Software for details."



question_type=$(zenity --list --radiolist --text \
"Hybrid means this PC will not only be the main server, but you will also 
hook it up to a TV and use it as a media director.

Core means it is a server only, and will not be used as a media station.

What do you want to install on this computer ?" \
--column "Choose" --column "Type" \
TRUE "Hybrid" \
FALSE "Core")


zenity --question --no-wrap --text \
"If you have an active internet connection, Pluto can use it to perform
various task : syncronize your computer clock, automaticaly send bug
reports, install aditional software, perform updates ...

You can also switch from online/offline mode later by using the web
administration interface

Do you have an active internet connection ?"
question_use_internet_connection=$?


question_netaddr=$(zenity --entry --text \
"Enter either 1, 2, or 3 numbers separated with periods, such as 192.168.1, 10.12, or 10
Press enter to use the default range of 192.168.80

What is the internal network adress that you want to use ?
" \
--entry-text "192.168.80")


zenity --question --no-wrap --text \
"
DHCP is a protocol like BOOTP that assigns IP addresses to clients based on lease times.
You need to answer 'Ok' below if you want Plug-and-Play or extra media directors !

Should Puto configure and run a DHCP server on this computer ?
"
question_run_dhcp=$?
}


function Setup_Apt_Conffiles {
## Setup apt sources.list
local Sources="# Pluto sources - start
deb http://archive.ubuntu.com/ubuntu edgy main restricted multiverse universe
deb file:/usr/pluto/deb-cache/ sarge main
# Pluto sources - end"
#echo "$Sources" >/etc/apt/sources.list

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

function Setup_Offline_Mode {
	if [[ "$question_use_internet_connection" == "1" ]] ;then
	        OfflineApt='
Acquire::http::timeout "-1";
Acquire::ftp::timeout "-1";
APT::Get::Download "false";
APT::Get::Fix-Missing "true";
APT::Get::quiet "1";
Dir::Etc::sourcelist "sources.list.offline";
'
        	mkdir -p /usr/pluto/var
	        echo -e $OfflineApt > /usr/pluto/var/apt.conf.offline
	fi

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
"
	echo "$PlutoConf" >/etc/pluto.conf
	if [[ "$question_use_internet_connection" == "1" ]] ;then
		echo -e "OfflineMode = true\n" >> /etc/pluto.conf
	else
		echo -e "OfflineMode = false\n" >> /etc/pluto.conf
	fi
	chmod 777 /etc/pluto.conf &>/dev/null
}

function Setpu_NIS {
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
	Core_DT=$(/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_Core | tee /dev/stderr | tail -1)
	Q="UPDATE Device SET Description='CORE' WHERE PK_Device='$Core_DT'"
	RunSQL "$Q"

	## Create a hybrid if needed
	if [[ $question_type == "Hybrid" ]] ;then
		Hybrid_DT=$(/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_MediaDirector -C "$Core_DT")
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

#        Q="REPLACE INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES('$CoreDev',32,'$NETsetting')"
#        RunSQL "$Q"
#        if [[ "$DHCP" != n && "$DHCP" != N ]]; then
#                Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
#                        VALUES($CoreDev, 28, '$DHCPsetting')"
#                RunSQL "$Q"
#        fi


	/usr/pluto/bin/Network_Setup.sh
	/usr/pluto/bin/DHCP_config.sh

}

Ask_Questions_Zenity
Setup_Apt_Conffiles
Setup_Offline_Mode
Setup_Pluto_Conf
Install_DCERouter
Create_And_Config_Devices
Configure_Network_Options

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/generateRcScripts.sh
/usr/pluto/bin/Timezone_Detect.sh

