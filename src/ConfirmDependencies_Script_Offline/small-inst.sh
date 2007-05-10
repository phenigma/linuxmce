#!/bin/bash -x

#<-mkr_B_etch_b->
exit 0
#<-mkr_B_etch_e->

exec &> >(tee /var/log/mce-etch-install.log)

function Setup_Apt() {
	sed -i '/cdrom/ d' /etc/apt/sources.list
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
Display = 0
SharedDesktop = 1
OfflineMode = false
LogLevels = 1,5,7,8
"
	echo "$PlutoConf" > /etc/pluto.conf
	chmod 777 /etc/pluto.conf &>/dev/null
}

function Install_DCERouter {
	StatsMessage "Installing MySQL Server"
	apt-get update
	apt-get -y -f install mysql-server || ExitInstaller "Failed to install mysql server"
	invoke-rc.d mysql start # Because of this : https://bugs.launchpad.net/bugs/107224

	StatsMessage "Installing LinuxMCE Base Software"
	apt-get -y -f install pluto-dcerouter || ExitInstaller "Failed to install and configure the base software"
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
		/usr/pluto/bin/CreateDevice -d $DEVICE_TEMPLATE_MediaDirector -C "$Core_PK_Device"
		Hybrid_DT=$(RunSQL "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DEVICE_TEMPLATE_MediaDirector' LIMIT 1")

		Q="UPDATE Device SET Description='The core/hybrid' WHERE PK_Device='$Hybrid_DT'"
		RunSQL "$Q"
 
		## Set UI interface
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Hybrid_DT' AND FK_DeviceTemplate=62"
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

function Setup_XOrg {
	## Make X accessible by Pluto software, for all existing and new users
	KDExhost="#!/bin/bash
	xhost +
	"

	for user in /home/*; do
		if [[ ! -d "$user" ]]; then
			continue
		fi
		Dir="$user/.kde/Autostart"
		mkdir -p "$Dir"
		echo "$KDExhost" >"$Dir/xhost"
		User="${user#/home/}"
		chown "$User.$User" "$Dir/xhost"
		chmod +x "$Dir/xhost"
	done

	Dir="/etc/skel/.kde/Autostart"
	echo "$KDExhost" >"$Dir/xhost"
	chmod +x "$Dir/xhost"

	## Add xrecord extention if missing
	if grep -q ".*Load.*\"record\"" /etc/X11/xorg.conf ;then
		echo "Found xrecord , skiping ..."
	else 
		sed -i 's/.*Section.*\"Module\"/Section "Module"\n Load "record"/g' /etc/X11/xorg.conf
	fi
}

function Setup_NIS {
	## Put a temporary nis config file that will prevent ypbind to start
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

c_deviceType=2
c_installUI=1

/usr/pluto/install/PreseedStage2.sh

Setup_NIS

Setup_Apt
Setup_Pluto_Conf
Install_DCERouter
Create_And_Config_Devices
Setup_XOrg

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/Timezone_Detect.sh
/usr/pluto/bin/Network_Setup.sh
/usr/pluto/bin/DHCP_config.sh

/home/via/ApplyViaPatch.sh

reboot
