#!/bin/bash

. /usr/pluto/install/Common.sh
DIR="/usr/pluto/install"

echo "** Initial config script"

# Ask the questions
Username=$(Ask "Pluto Username")
Password=$(Ask "Pluto Password")
Room=$(Ask "Room name")
Type=$(Ask "Core or Hybrid? [C/h]")
DHCP=$(Ask "DHCP: none/pluto/all [n/p/A]")
if [[ -z "$DHCP" ]]; then
	DHCP=A
fi

RestoreCoreConf()
{
	[[ -d /home/backup ]] || return 0
	[[ -f /home/backup/pluto.conf-Core ]] || return 0

	sed "$NoSpace" /home/backup/pluto.conf-Core | egrep -v "^#|^//" >/etc/pluto.conf
	while read line; do
		eval "local $line" &>/dev/null
	done </etc/pluto.conf
	export Device="$PK_Device"
	export Code="$Activation_Code"
}

# /etc/apt/apt.conf.d/30pluto
pluto_apt_conf='// Pluto apt conf add-on
Acquire::http::Proxy "http://127.0.0.1:8123";
Acquire::ftp::Proxy "ftp://127.0.0.1:8124";
Acquire::ftp::ProxyLogin { "USER $(SITE_USER)@$(SITE):$(SITE_PORT)"; "PASS $(SITE_PASS)"; };
Acquire::http::Proxy::dcerouter "DIRECT";
Apt::Cache-Limit "12582912";
'

# Install critical (Deb talk: essential) packages
CriticalPattern="/usr/pluto/install/deb-critical/*"
CriticalPkgs=$(echo $CriticalPattern)
if [[ -n "$CriticalPkgs" ]]; then
	if dpkg -i $CriticalPattern; then
		echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto
		echo -n "$polipo_conf" >/etc/polipo/config
		echo -n "$frox_conf" >/etc/frox.conf
		/usr/sbin/adduser --system --group --home /var/cache/frox --disabled-login --disabled-password frox
		echo "RUN_DAEMON=yes" >/etc/default/frox
		/etc/init.d/frox restart
		/etc/init.d/polipo restart
	else
		echo "* ERROR * Complete proxy combination is not available. Installation can't continue"
		read
		exit 1
	fi
fi
RestoreCoreConf
WhereCode="in pluto.conf backup"

# Pluto installation
Activation_Code=1111

Sources="# Pluto sources - start
deb file:/usr/pluto/deb-cache/ sarge main
deb http://deb.plutohome.com/debian/ sarge main non-free contrib
deb http://deb.plutohome.com/debian/ unstable mythtv
deb http://www.yttron.as.ro/ sarge main
# Pluto sources - end"

PlutoConf="# Pluto config file
MySqlHost = localhost
MySqlUser = root
MySqlPassword =
MySqlDBName = pluto_main
DCERouter = localhost
MySqlPort = 3306
DCERouterPort = 3450
PK_Device = 1
Activation_Code = $Activation_Code
PK_Installation = 1
PK_Users = 1"

echo "$PlutoConf" >/etc/pluto.conf

echo "$Sources" >/etc/apt/sources.list
aptitude update
if ! aptitude -y -f install pluto-dcerouter; then
	echo "Installation failed"
	exit 1
fi

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# "Initial config"
# Create installation
Q="INSERT INTO Installation(Description, ActivationCode) VALUES('Pluto', '$Activation_Code')"
RunSQL "$Q"

# Create user
PasswordUnix=$(mkpasswd -H md5 "$Password")
PasswordSamba=$(/usr/pluto/bin/smbpass.pl "$Password")

sqlUsername="${Username//\'/\'}"
sqlPassword="${Password//\'/\'}"
sqlPasswordUnix="${PasswordUnix//\'/\'}"
sqlPasswordSamba="${PasswordSamba//\'/\'}"
Q="INSERT INTO Users(UserName, Password, Password_Unix, Password_Samba)
	VALUES('$sqlUsername', MD5('$sqlPassword'), '$sqlPasswordUnix', '$sqlPasswordSamba')"
RunSQL "$Q"

# Add user to installation
Q="INSERT INTO Installation_Users(FK_Installation, FK_Users, userCanModifyInstallation)
	VALUES(1, 1, 1)"
RunSQL "$Q"

# Create room
sqlRoom="${Room//\'/\'}"
Q="INSERT INTO Room(FK_Installation, FK_RoomType, Description)
	VALUES(1, 9, '$sqlRoom')"
RunSQL "$Q"

# Add Core
CoreDev=$(NewDev -d 7)
Q="UPDATE Device SET Description='CORE', FK_Room=1 WHERE PK_Device='$CoreDev'"
RunSQL "$Q"

# Add Core options
Q='SELECT FK_DeviceTemplate FROM InstallWizard WHERE `Default`=1 AND Step=5'
R=$(RunSQL "$Q")

for DevT in $R; do
	Dev=$(NewDev -d "$DevT")
done

# Create hybrid
if [[ "$Type" == "H" || "$Type" == "h" ]]; then
	HybDev=$(NewDev -d 28 -C "$CoreDev")
	Q="UPDATE Device SET Description='The core/hybrid', FK_Room=1 WHERE PK_Device='$HybDev'"
	RunSQL "$Q"

	# Add hybrid options
	Q='SELECT FK_DeviceTemplate FROM InstallWizard WHERE `Default`=1 AND Step=6'
	R=$(RunSQL "$Q")

	for DevT in $R; do
		Dev=$(NewDev -d "$DevT")
	done
fi

DHCPsetting=
if [[ "$DHCP" == p || "$DHCP" == P ]]; then
	DHCPsetting="192.168.80.2-192.168.80.254"
elif [[ "$DHCP" == a || "$DHCP" == A ]]; then
	DHCPsetting="192.168.80.2-192.168.80.128,192.168.80.129-192.168.80.254"
fi

if [[ -n "$DHCPsetting" ]]; then
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES($CoreDev, 28, '$DHCPsetting')"
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

# Add to Installation_Users
Q4="INSERT INTO Installation_Users(FK_Installation,FK_Users,userCanModifyInstallation)
SELECT PK_Installation,PK_Users,1
FROM Installation
JOIN Users;"

echo "$Q4" | /usr/bin/mysql pluto_main

# Update startup scripts
/usr/pluto/bin/Update_StartupScrips.sh

# Remove installation incomplete flag
rm /usr/pluto/install/.notdone

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

clear

if [[ -d /home/backup && -f /home/backup/entire_database.sql ]]; then
	echo "Restoring database"
	mysql < /home/backup/entire_database.sql
fi

/usr/pluto/install/Initial_Config_Finish.sh
#init q
exit 0
